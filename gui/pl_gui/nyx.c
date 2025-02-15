/*
 * Copyright (c) 2018 naehrwert
 *
 * Copyright (c) 2018-2019 CTCaer
 * Copyright (c) 2020 Storm
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdlib.h>

#include <memory_map.h>

#include "config.h"
#include <display/di.h>
#include <gfx_utils.h>
#include "hos/hos.h"
#include <ianos/ianos.h>
#include <libs/compr/blz.h>
#include <libs/fatfs/ff.h>
#include <mem/heap.h>
#include <mem/minerva.h>
#include <mem/sdram.h>
#include <power/max77620.h>
#include <soc/clock.h>
#include <soc/bpmp.h>
#include <soc/fuse.h>
#include <soc/gpio.h>
#include <soc/hw_init.h>
#include <soc/i2c.h>
#include <soc/pinmux.h>
#include <soc/pmc.h>
#include <soc/t210.h>
#include <soc/uart.h>
#include <storage/nx_sd.h>
#include <storage/sdmmc.h>
#include <utils/btn.h>
#include <utils/dirlist.h>
#include <utils/list.h>
#include <utils/util.h>

#include "frontend/fe_emmc_tools.h"
#include "frontend/gui.h"

#ifdef MENU_LOGO_ENABLE
u8 *Kc_MENU_LOGO;
#endif //MENU_LOGO_ENABLE

nyx_config n_cfg;
hekate_config h_cfg;

const volatile ipl_ver_meta_t __attribute__((section ("._ipl_version"))) ipl_ver = {
	.magic = NYX_MAGIC,
	.version = (NYX_VER_MJ + '0') | ((NYX_VER_MN + '0') << 8) | ((NYX_VER_HF + '0') << 16),
	.rsvd0 = 0,
	.rsvd1 = 0
};

volatile nyx_storage_t *nyx_str = (nyx_storage_t *)NYX_STORAGE_ADDR;
volatile boot_cfg_t *b_cfg;

char *emmcsn_path_impl(char *path, char *sub_dir, char *filename, sdmmc_storage_t *storage)
{
	static char emmc_sn[9] = {0};

	// Check if eMMC S/N storage has valid data and skip parsing in that case.
	if (emmc_sn[0] && strcmp(emmc_sn, "00000000"))
		goto create_dir;

	// Get actual eMMC S/N.
	if (!storage)
	{
		sdmmc_t sdmmc;
		sdmmc_storage_t storage2;

		if (!sdmmc_storage_init_mmc(&storage2, &sdmmc, SDMMC_BUS_WIDTH_8, SDHCI_TIMING_MMC_HS400))
			strcpy(emmc_sn, "00000000");
		else
		{
			itoa(storage2.cid.serial, emmc_sn, 16);
			sdmmc_storage_end(&storage2);
		}
	}
	else
		itoa(storage->cid.serial, emmc_sn, 16);

create_dir:
	// Check if only eMMC S/N was requested.
	if (!path)
		return emmc_sn;

	// Create main folder.
	strcpy(path, "backup");
	f_mkdir(path);

	// Create eMMC S/N folder.
	strcat(path, "/");
	strcat(path, emmc_sn);
	f_mkdir(path);

	// Create sub folder if defined. Dir slash must be included.
	strcat(path, sub_dir);  // Can be a null-terminator.
	if (strlen(sub_dir))
		f_mkdir(path);

	// Add filename.
	strcat(path, "/");
	strcat(path, filename); // Can be a null-terminator.

	return emmc_sn;
}

// This is a safe and unused DRAM region for our payloads.
#define RELOC_META_OFF      0x7C
#define PATCHED_RELOC_SZ    0x94
#define PATCHED_RELOC_STACK 0x40007000
#define PATCHED_RELOC_ENTRY 0x40010000
#define EXT_PAYLOAD_ADDR    0xC0000000
#define RCM_PAYLOAD_ADDR    (EXT_PAYLOAD_ADDR + ALIGN(PATCHED_RELOC_SZ, 0x10))
#define COREBOOT_END_ADDR   0xD0000000
#define CBFS_DRAM_EN_ADDR   0x4003e000
#define  CBFS_DRAM_MAGIC    0x4452414D // "DRAM"

static void *coreboot_addr;

void reloc_patcher(u32 payload_dst, u32 payload_src, u32 payload_size)
{
	memcpy((u8 *)payload_src, (u8 *)nyx_str->hekate, PATCHED_RELOC_SZ);

	volatile reloc_meta_t *relocator = (reloc_meta_t *)(payload_src + RELOC_META_OFF);

	relocator->start = payload_dst - ALIGN(PATCHED_RELOC_SZ, 0x10);
	relocator->stack = PATCHED_RELOC_STACK;
	relocator->end   = payload_dst + payload_size;
	relocator->ep    = payload_dst;

	if (payload_size == 0x7000)
	{
		memcpy((u8 *)(payload_src + ALIGN(PATCHED_RELOC_SZ, 0x10)), coreboot_addr, 0x7000); //Bootblock
		*(vu32 *)CBFS_DRAM_EN_ADDR = CBFS_DRAM_MAGIC;
	}
}

lv_res_t launch_payload(lv_obj_t *list)
{
	const char *filename = lv_list_get_btn_text(list);

	if (!filename || !filename[0])
		goto out;

	char path[128];

	strcpy(path,"bootloader/payloads/");
	strcat(path, filename);

	if (sd_mount())
	{
		FIL fp;
		if (f_open(&fp, path, FA_READ))
		{
			EPRINTFARGS("Payload file is missing!\n(%s)", path);

			goto out;
		}

		// Read and copy the payload to our chosen address
		void *buf;
		u32 size = f_size(&fp);

		if (size < 0x30000)
			buf = (void *)RCM_PAYLOAD_ADDR;
		else
		{
			coreboot_addr = (void *)(COREBOOT_END_ADDR - size);
			buf = coreboot_addr;
			if (h_cfg.t210b01)
			{
				f_close(&fp);

				EPRINTF("Coreboot not allowed on Mariko!");

				goto out;
			}
		}

		if (f_read(&fp, buf, size, NULL))
		{
			f_close(&fp);

			goto out;
		}

		f_close(&fp);

		sd_end();

		if (size < 0x30000)
		{
			reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, ALIGN(size, 0x10));
			hw_reinit_workaround(false, byte_swap_32(*(u32 *)(buf + size - sizeof(u32))));
		}
		else
		{
			reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, 0x7000);
			hw_reinit_workaround(true, 0);
		}

		void (*ext_payload_ptr)() = (void *)EXT_PAYLOAD_ADDR;

		// Some cards (Sandisk U1), do not like a fast power cycle. Wait min 100ms.
		sdmmc_storage_init_wait_sd();

		// Launch our payload.
		(*ext_payload_ptr)();
	}

out:
	sd_unmount();

	return LV_RES_OK;
}

//Payload loader label Button
lv_res_t launch_payload_btn(lv_obj_t* obj)
{
	lv_obj_t label = lv_obj_get_child(obj, NULL)[0];
	char* path = lv_label_get_text(&label);

	if (!path || !path[0])
		goto out;

	if (sd_mount())
	{
		FIL fp;
		if (f_open(&fp, path, FA_READ))
		{
			EPRINTFARGS("Payload file is missing!\n(%s)", path);

			goto out;
		}

		// Read and copy the payload to our chosen address
		void* buf;
		u32 size = f_size(&fp);

		if (size < 0x30000)
			buf = (void*)RCM_PAYLOAD_ADDR;
		else
		{
			coreboot_addr = (void*)(COREBOOT_END_ADDR - size);
			buf = coreboot_addr;
			if (h_cfg.t210b01)
			{
				f_close(&fp);

				EPRINTF("Coreboot not allowed on Mariko!");

				goto out;
			}
		}

		if (f_read(&fp, buf, size, NULL))
		{
			f_close(&fp);

			goto out;
		}

		f_close(&fp);

		sd_end();

		if (size < 0x30000)
		{
			reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, ALIGN(size, 0x10));
			hw_reinit_workaround(false, byte_swap_32(*(u32*)(buf + size - sizeof(u32))));
		}
		else
		{
			reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, 0x7000);
			hw_reinit_workaround(true, 0);
		}

		void (*ext_payload_ptr)() = (void*)EXT_PAYLOAD_ADDR;

		// Some cards (Sandisk U1), do not like a fast power cycle. Wait min 100ms.
		sdmmc_storage_init_wait_sd();

		// Launch our payload.
		(*ext_payload_ptr)();
	}

out:
	sd_unmount();

	return LV_RES_OK;
}

void load_saved_configuration()
{
	LIST_INIT(ini_sections);
	LIST_INIT(ini_nyx_sections);

	// Load hekate configuration.
	if (ini_parse(&ini_sections, "bootloader/hekate_ipl.ini", false))
	{
		LIST_FOREACH_ENTRY(ini_sec_t, ini_sec, &ini_sections, link)
		{
			// Only parse config section.
			if (ini_sec->type == INI_CHOICE && !strcmp(ini_sec->name, "config"))
			{
				LIST_FOREACH_ENTRY(ini_kv_t, kv, &ini_sec->kvs, link)
				{
					if (!strcmp("autoboot", kv->key))
						h_cfg.autoboot = atoi(kv->val);
					else if (!strcmp("autoboot_list", kv->key))
						h_cfg.autoboot_list = atoi(kv->val);
					else if (!strcmp("bootwait", kv->key))
						h_cfg.bootwait = atoi(kv->val);
					else if (!strcmp("backlight", kv->key))
					{
						h_cfg.backlight = atoi(kv->val);
						if (h_cfg.backlight <= 20)
							h_cfg.backlight = 30;
					}
					else if (!strcmp("autohosoff", kv->key))
						h_cfg.autohosoff = atoi(kv->val);
					else if (!strcmp("autonogc", kv->key))
						h_cfg.autonogc = atoi(kv->val);
					else if (!strcmp("updater2p", kv->key))
						h_cfg.updater2p = atoi(kv->val);
					else if (!strcmp("bootprotect", kv->key))
						h_cfg.bootprotect = atoi(kv->val);
				}

				break;
			}
		}
	}

	// Load Nyx configuration.
	if (ini_parse(&ini_nyx_sections, "bootloader/nyx.ini", false))
	{
		LIST_FOREACH_ENTRY(ini_sec_t, ini_sec, &ini_nyx_sections, link)
		{
			// Only parse config section.
			if (ini_sec->type == INI_CHOICE && !strcmp(ini_sec->name, "config"))
			{
				LIST_FOREACH_ENTRY(ini_kv_t, kv, &ini_sec->kvs, link)
				{
					if (!strcmp("themecolor", kv->key))
						n_cfg.themecolor = atoi(kv->val);
					else if (!strcmp("timeoff", kv->key))
						n_cfg.timeoff = strtol(kv->val, NULL, 16);
					else if (!strcmp("homescreen", kv->key))
						n_cfg.home_screen = atoi(kv->val);
					else if (!strcmp("verification", kv->key))
						n_cfg.verification = atoi(kv->val);
					else if (!strcmp("umsemmcrw", kv->key))
						n_cfg.ums_emmc_rw = atoi(kv->val) == 1;
					else if (!strcmp("jcdisable", kv->key))
						n_cfg.jc_disable = atoi(kv->val) == 1;
					else if (!strcmp("newpowersave", kv->key))
						n_cfg.new_powersave = atoi(kv->val) == 1;
				}

				break;
			}
		}
	}
}

#define EXCP_EN_ADDR   0x4003FFFC
#define  EXCP_MAGIC 0x30505645      // EVP0
#define EXCP_TYPE_ADDR 0x4003FFF8
#define  EXCP_TYPE_RESET 0x545352   // RST
#define  EXCP_TYPE_UNDEF 0x464455   // UDF
#define  EXCP_TYPE_PABRT 0x54424150 // PABT
#define  EXCP_TYPE_DABRT 0x54424144 // DABT
#define EXCP_LR_ADDR   0x4003FFF4

static void _show_errors()
{
	u32 *excp_enabled = (u32 *)EXCP_EN_ADDR;
	u32 *excp_type = (u32 *)EXCP_TYPE_ADDR;
	u32 *excp_lr = (u32 *)EXCP_LR_ADDR;

	if (*excp_enabled == EXCP_MAGIC)
	{
		gfx_clear_grey(0);
		gfx_con_setpos(0, 0);
		display_backlight_brightness(100, 1000);

		display_activate_console();

		WPRINTFARGS("An exception occurred (LR %08X):\n", *excp_lr);
		switch (*excp_type)
		{
		case EXCP_TYPE_RESET:
			WPRINTF("RESET");
			break;
		case EXCP_TYPE_UNDEF:
			WPRINTF("UNDEF");
			break;
		case EXCP_TYPE_PABRT:
			WPRINTF("PABRT");
			break;
		case EXCP_TYPE_DABRT:
			WPRINTF("DABRT");
			break;
		}
		WPRINTF("\n");

		// Clear the exception.
		*excp_lr = 0;
		*excp_type = 0;
		*excp_enabled = 0;

		WPRINTF("Press any key...");

		msleep(2000);
		btn_wait();

		reload_nyx();
	}
}

void nyx_init_load_res()
{
	bpmp_mmu_enable();
	bpmp_clk_rate_get();
	bpmp_clk_rate_set(BPMP_CLK_DEFAULT_BOOST);

	// Set bootloader's default configuration.
	set_default_configuration();
	set_nyx_default_configuration();

	// Set display id from previous initialization.
	display_set_decoded_panel_id(nyx_str->info.disp_id);

	// Initialize gfx console.
	gfx_init_ctxt((u32 *)LOG_FB_ADDRESS, 1280, 656, 656);
	gfx_con_init();

	// Show exception errors if any.
	_show_errors();

	sd_mount();

	// Train DRAM and switch to max frequency.
	minerva_init();

	FIL fp;
	f_open(&fp, "argon/sys/res.emunsw", FA_READ);
	f_read(&fp, (void*)NYX_RES_ADDR, f_size(&fp), NULL);
	f_close(&fp);

	hekate_bg = bmp_to_lvimg_obj("argon/background.bmp");

	sd_unmount();
}

#if (LV_LOG_PRINTF == 1)
	#include <soc/clock.h>
	#include <soc/gpio.h>
	#include <soc/pinmux.h>
#endif

//Modifiziert f�r mich
void ipl_main()
{
	//Tegra/Horizon configuration goes to 0x80000000+, package2 goes to 0xA9800000, we place our heap in between.
	heap_init(IPL_HEAP_START);


	b_cfg = (boot_cfg_t *)(nyx_str->hekate + 0x94);

	// Important: Preserve version header!
	__asm__ ("" : : "" (ipl_ver));

#ifdef DEBUG_UART_PORT
	#if DEBUG_UART_PORT == UART_B
		gpio_config(GPIO_PORT_G, GPIO_PIN_0, GPIO_MODE_SPIO);
		gpio_config(GPIO_PORT_D, GPIO_PIN_1, GPIO_MODE_GPIO);
	#endif
	#if DEBUG_UART_PORT == UART_C
		gpio_config(GPIO_PORT_G, GPIO_PIN_0, GPIO_MODE_GPIO);
		gpio_config(GPIO_PORT_D, GPIO_PIN_1, GPIO_MODE_SPIO);
	#endif
	pinmux_config_uart(DEBUG_UART_PORT);
	clock_enable_uart(DEBUG_UART_PORT);
	uart_init(DEBUG_UART_PORT, DEBUG_UART_BAUDRATE);
	uart_invert(DEBUG_UART_PORT, DEBUG_UART_INVERT, UART_INVERT_TXD);

	uart_send(DEBUG_UART_PORT, (u8 *)"hekate-NYX: Hello!\r\n", 20);
	uart_wait_idle(DEBUG_UART_PORT, UART_TX_IDLE);
#endif

	// Initialize the rest of hw and load nyx's resources.
	nyx_init_load_res();

	gui_load_and_run();//Men� laden

	// Halt BPMP if we managed to get out of execution.
	while (true)
		bpmp_halt();
}

/*
 * Copyright (c) 2018 naehrwert
 *
 * Copyright (c) 2018-2020 CTCaer
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
#include "gfx/logos.h"
#include "gfx/tui.h"
#include "hos/hos.h"
#include "hos/secmon_exo.h"
#include "hos/sept.h"
#include <ianos/ianos.h>
#include <libs/compr/blz.h>
#include <libs/fatfs/ff.h>
#include <mem/heap.h>
#include <mem/minerva.h>
#include <mem/sdram.h>
#include <power/bq24193.h>
#include <power/max17050.h>
#include <power/max77620.h>
#include <power/max7762x.h>
#include <rtc/max77620-rtc.h>
#include <soc/bpmp.h>
#include <soc/fuse.h>
#include <soc/hw_init.h>
#include <soc/i2c.h>
#include <soc/pmc.h>
#include <soc/t210.h>
#include <soc/uart.h>
#include "storage/emummc.h"
#include "storage/nx_emmc.h"
#include <storage/nx_sd.h>
#include <storage/sdmmc.h>
#include <utils/btn.h>
#include <utils/dirlist.h>
#include <utils/list.h>
#include <utils/util.h>

#include "frontend/fe_emmc_tools.h"
#include "frontend/fe_tools.h"
#include "frontend/fe_info.h"

hekate_config h_cfg;
boot_cfg_t __attribute__((section ("._boot_cfg"))) b_cfg;
const volatile ipl_ver_meta_t __attribute__((section ("._ipl_version"))) ipl_ver = {
	.magic = BL_MAGIC,
	.version = (BL_VER_MJ + '0') | ((BL_VER_MN + '0') << 8) | ((BL_VER_HF + '0') << 16),
	.rsvd0 = 0,
	.rsvd1 = 0
};

volatile nyx_storage_t *nyx_str = (nyx_storage_t *)NYX_STORAGE_ADDR;

void emmcsn_path_impl(char *path, char *sub_dir, char *filename, sdmmc_storage_t *storage)
{
	sdmmc_storage_t storage2;
	sdmmc_t sdmmc;
	char emmcSN[9];
	bool init_done = false;

	memcpy(path, "backup", 7);
	f_mkdir(path);

	if (!storage)
	{
		if (!sdmmc_storage_init_mmc(&storage2, &sdmmc, SDMMC_BUS_WIDTH_8, SDHCI_TIMING_MMC_HS400))
			memcpy(emmcSN, "00000000", 9);
		else
		{
			init_done = true;
			itoa(storage2.cid.serial, emmcSN, 16);
		}
	}
	else
		itoa(storage->cid.serial, emmcSN, 16);

	u32 sub_dir_len = strlen(sub_dir);   // Can be a null-terminator.
	u32 filename_len = strlen(filename); // Can be a null-terminator.

	memcpy(path + strlen(path), "/", 2);
	memcpy(path + strlen(path), emmcSN, 9);
	f_mkdir(path);
	memcpy(path + strlen(path), sub_dir, sub_dir_len + 1);
	if (sub_dir_len)
		f_mkdir(path);
	memcpy(path + strlen(path), "/", 2);
	memcpy(path + strlen(path), filename, filename_len + 1);

	if (init_done)
		sdmmc_storage_end(&storage2);
}

void check_power_off_from_hos()
{
	// Power off on AutoRCM wakeup from HOS shutdown. For modchips/dongles.
	u8 hosWakeup = i2c_recv_byte(I2C_5, MAX77620_I2C_ADDR, MAX77620_REG_IRQTOP);
	if (hosWakeup & MAX77620_IRQ_TOP_RTC_MASK)
	{
		sd_end();

		// Stop the alarm, in case we injected too fast.
		max77620_rtc_stop_alarm();

		if (h_cfg.autohosoff == 1)
		{
			gfx_clear_grey(0x1B);
			u8 *BOOTLOGO = (void *)malloc(0x4000);
			blz_uncompress_srcdest(BOOTLOGO_BLZ, SZ_BOOTLOGO_BLZ, BOOTLOGO, SZ_BOOTLOGO);
			gfx_set_rect_grey(BOOTLOGO, X_BOOTLOGO, Y_BOOTLOGO, 326, 544);

			display_backlight_brightness(10, 5000);
			display_backlight_brightness(100, 25000);
			msleep(600);
			display_backlight_brightness(0, 20000);
		}
		power_set_state(POWER_OFF_RESET);
	}
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
	memcpy((u8 *)payload_src, (u8 *)IPL_LOAD_ADDR, PATCHED_RELOC_SZ);

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

bool is_ipl_updated(void *buf, char *path, bool force)
{
	ipl_ver_meta_t *update_ft = (ipl_ver_meta_t *)(buf + PATCHED_RELOC_SZ + sizeof(boot_cfg_t));

	bool magic_valid = update_ft->magic == ipl_ver.magic;
	bool force_update = force && !magic_valid;
	bool is_valid_old = magic_valid && (byte_swap_32(update_ft->version) < byte_swap_32(ipl_ver.version));

	// Check if newer version.
	if (!force && magic_valid)
		if (byte_swap_32(update_ft->version) > byte_swap_32(ipl_ver.version))
			return false;

	// Update if old or broken.
	if (force_update || is_valid_old)
	{
		FIL fp;
		volatile reloc_meta_t *reloc = (reloc_meta_t *)(IPL_LOAD_ADDR + RELOC_META_OFF);
		boot_cfg_t *tmp_cfg = malloc(sizeof(boot_cfg_t));
		memset(tmp_cfg, 0, sizeof(boot_cfg_t));

		f_open(&fp, path, FA_WRITE | FA_CREATE_ALWAYS);
		f_write(&fp, (u8 *)reloc->start, reloc->end - reloc->start, NULL);

		// Write needed tag in case injected ipl uses old versioning.
		f_write(&fp, "ICTC49", 6, NULL);

		// Reset boot storage configuration.
		f_lseek(&fp, PATCHED_RELOC_SZ);
		f_write(&fp, tmp_cfg, sizeof(boot_cfg_t), NULL);

		f_close(&fp);
		free(tmp_cfg);
	}

	return true;
}

int launch_payload(char *path, bool update)
{
	if (!update)
		gfx_clear_grey(0x1B);
	gfx_con_setpos(0, 0);

	if (sd_mount())
	{
		FIL fp;
		if (f_open(&fp, path, FA_READ))
		{
			gfx_con.mute = 0;
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

				gfx_con.mute = 0;
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

		if (update && is_ipl_updated(buf, path, false))
			goto out;

		sd_end();

		if (size < 0x30000)
		{
			if (update)
				memcpy((u8 *)(RCM_PAYLOAD_ADDR + PATCHED_RELOC_SZ), &b_cfg, sizeof(boot_cfg_t)); // Transfer boot cfg.
			else
				reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, ALIGN(size, 0x10));

			hw_reinit_workaround(false, byte_swap_32(*(u32 *)(buf + size - sizeof(u32))));
		}
		else
		{
			reloc_patcher(PATCHED_RELOC_ENTRY, EXT_PAYLOAD_ADDR, 0x7000);
			hw_reinit_workaround(true, 0);
		}

		// Some cards (Sandisk U1), do not like a fast power cycle. Wait min 100ms.
		sdmmc_storage_init_wait_sd();

		void (*ext_payload_ptr)() = (void *)EXT_PAYLOAD_ADDR;
		void (*update_ptr)() = (void *)RCM_PAYLOAD_ADDR;

		// Launch our payload.
		if (!update)
			(*ext_payload_ptr)();
		else
		{
			EMC(EMC_SCRATCH0) |= EMC_HEKA_UPD;
			(*update_ptr)();
		}
	}

out:
	if (!update)
		sd_end();

	return 1;
}

/*void auto_launch_update()
{
	if (EMC(EMC_SCRATCH0) & EMC_HEKA_UPD)
		EMC(EMC_SCRATCH0) &= ~EMC_HEKA_UPD;
	else if (sd_mount())
	{
		// Check if update.bin exists and is newer and launch it. Otherwise create it.
		if (!f_stat("bootloader/update.bin", NULL))
			launch_payload("bootloader/update.bin", true);
		else
		{
			u8 *buf = calloc(0x200, 1);
			is_ipl_updated(buf, "bootloader/update.bin", true);
			free(buf);
		}
	}
}*/

void launch_tools()
{
	u8 max_entries = 61;
	char *filelist = NULL;
	char *file_sec = NULL;
	char *dir = NULL;

	ment_t *ments = (ment_t *)malloc(sizeof(ment_t) * (max_entries + 3));

	gfx_clear_grey(0x1B);
	gfx_con_setpos(0, 0);

	if (sd_mount())
	{
		dir = (char *)malloc(256);

		memcpy(dir, "bootloader/payloads", 20);

		filelist = dirlist(dir, NULL, false, false);

		u32 i = 0;

		if (filelist)
		{
			// Build configuration menu.
			ments[0].type = MENT_BACK;
			ments[0].caption = "Back";
			ments[1].type = MENT_CHGLINE;

			while (true)
			{
				if (i > max_entries || !filelist[i * 256])
					break;
				ments[i + 2].type = INI_CHOICE;
				ments[i + 2].caption = &filelist[i * 256];
				ments[i + 2].data = &filelist[i * 256];

				i++;
			}
		}

		if (i > 0)
		{
			memset(&ments[i + 2], 0, sizeof(ment_t));
			menu_t menu = { ments, "Choose a file to launch", 0, 0 };

			file_sec = (char *)tui_do_menu(&menu);

			if (!file_sec)
			{
				free(ments);
				free(dir);
				free(filelist);
				sd_end();

				return;
			}
		}
		else
			EPRINTF("No payloads or modules found.");

		free(ments);
		free(filelist);
	}
	else
	{
		free(ments);
		goto out;
	}

	if (file_sec)
	{
		memcpy(dir + strlen(dir), "/", 2);
		memcpy(dir + strlen(dir), file_sec, strlen(file_sec) + 1);

		launch_payload(dir, false);
		EPRINTF("Failed to launch payload.");
	}

out:
	sd_end();
	free(dir);

	btn_wait();
}

void ini_list_launcher()
{
	u8 max_entries = 61;
	char *payload_path = NULL;
	char *emummc_path = NULL;

	ini_sec_t *cfg_sec = NULL;
	LIST_INIT(ini_list_sections);

	gfx_clear_grey(0x1B);
	gfx_con_setpos(0, 0);

	if (sd_mount())
	{
		if (ini_parse(&ini_list_sections, "bootloader/ini", true))
		{
			// Build configuration menu.
			ment_t *ments = (ment_t *)malloc(sizeof(ment_t) * (max_entries + 3));
			ments[0].type = MENT_BACK;
			ments[0].caption = "Back";
			ments[1].type = MENT_CHGLINE;

			u32 i = 2;
			LIST_FOREACH_ENTRY(ini_sec_t, ini_sec, &ini_list_sections, link)
			{
				if (!strcmp(ini_sec->name, "config") ||
					ini_sec->type == INI_COMMENT || ini_sec->type == INI_NEWLINE)
					continue;
				ments[i].type = ini_sec->type;
				ments[i].caption = ini_sec->name;
				ments[i].data = ini_sec;
				if (ini_sec->type == MENT_CAPTION)
					ments[i].color = ini_sec->color;
				i++;

				if ((i - 1) > max_entries)
					break;
			}
			if (i > 2)
			{
				memset(&ments[i], 0, sizeof(ment_t));
				menu_t menu = {
					ments, "Launch ini configurations", 0, 0
				};

				cfg_sec = (ini_sec_t *)tui_do_menu(&menu);

				if (cfg_sec)
				{
					u32 non_cfg = 1;
					for (int j = 2; j < i; j++)
					{
						if (ments[j].type != INI_CHOICE)
							non_cfg++;

						if (ments[j].data == cfg_sec)
						{
							b_cfg.boot_cfg = BOOT_CFG_FROM_LAUNCH;
							b_cfg.autoboot = j - non_cfg;
							b_cfg.autoboot_list = 1;

							break;
						}
					}
				}

				payload_path = ini_check_payload_section(cfg_sec);

				if (cfg_sec)
				{
					LIST_FOREACH_ENTRY(ini_kv_t, kv, &cfg_sec->kvs, link)
					{
						if (!strcmp("emummc_force_disable", kv->key))
							h_cfg.emummc_force_disable = atoi(kv->val);
						else if (!strcmp("emupath", kv->key))
							emummc_path = kv->val;
					}
				}

				if (emummc_path && !emummc_set_path(emummc_path))
				{
					EPRINTF("emupath is wrong!");
					goto wrong_emupath;
				}

				if (cfg_sec && !payload_path)
					check_sept(cfg_sec);

				if (!cfg_sec)
				{
					free(ments);

					return;
				}
			}
			else
				EPRINTF("No extra configs found.");
			free(ments);
		}
		else
			EPRINTF("Could not find any ini\nin bootloader/ini!");
	}

	if (!cfg_sec)
		goto out;

	if (payload_path)
	{
		launch_payload(payload_path, false);
		EPRINTF("Failed to launch payload.");
		free(payload_path);
	}
	else if (!hos_launch(cfg_sec))
	{
wrong_emupath:
		EPRINTF("Failed to launch HOS.");
		if (emummc_path)
		{
			sd_mount();
			emummc_load_cfg();
		}
	}

out:

	btn_wait();
}

void launch_firmware()
{
	u8 max_entries = 61;
	char *payload_path = NULL;
	char *emummc_path = NULL;

	ini_sec_t *cfg_sec = NULL;
	LIST_INIT(ini_sections);

	gfx_clear_grey(0x1B);
	gfx_con_setpos(0, 0);

	if (sd_mount())
	{
		if (ini_parse(&ini_sections, "bootloader/hekate_ipl.ini", false))
		{
			// Build configuration menu.
			ment_t *ments = (ment_t *)malloc(sizeof(ment_t) * (max_entries + 6));
			ments[0].type = MENT_BACK;
			ments[0].caption = "Back";
			ments[1].type = MENT_CHGLINE;

			ments[2].type = MENT_HANDLER;
			ments[2].caption = "Payloads...";
			ments[2].handler = launch_tools;
			ments[3].type = MENT_HANDLER;
			ments[3].caption = "More configs...";
			ments[3].handler = ini_list_launcher;

			ments[4].type = MENT_CHGLINE;

			u32 i = 5;
			LIST_FOREACH_ENTRY(ini_sec_t, ini_sec, &ini_sections, link)
			{
				if (!strcmp(ini_sec->name, "config") ||
					ini_sec->type == INI_COMMENT || ini_sec->type == INI_NEWLINE)
					continue;
				ments[i].type = ini_sec->type;
				ments[i].caption = ini_sec->name;
				ments[i].data = ini_sec;
				if (ini_sec->type == MENT_CAPTION)
					ments[i].color = ini_sec->color;
				i++;

				if ((i - 4) > max_entries)
					break;
			}
			if (i < 6)
			{
				ments[i].type = MENT_CAPTION;
				ments[i].caption = "No main configs found...";
				ments[i].color = 0xFFFFDD00;
				i++;
			}
			memset(&ments[i], 0, sizeof(ment_t));
			menu_t menu = {
				ments, "Launch configurations", 0, 0
			};

			cfg_sec = (ini_sec_t *)tui_do_menu(&menu);

			if (cfg_sec)
			{
				u8 non_cfg = 4;
				for (int j = 5; j < i; j++)
				{
					if (ments[j].type != INI_CHOICE)
						non_cfg++;
					if (ments[j].data == cfg_sec)
					{
						b_cfg.boot_cfg = BOOT_CFG_FROM_LAUNCH;
						b_cfg.autoboot = j - non_cfg;
						b_cfg.autoboot_list = 0;

						break;
					}
				}
			}

			payload_path = ini_check_payload_section(cfg_sec);

			if (cfg_sec)
			{
				LIST_FOREACH_ENTRY(ini_kv_t, kv, &cfg_sec->kvs, link)
				{
					if (!strcmp("emummc_force_disable", kv->key))
						h_cfg.emummc_force_disable = atoi(kv->val);
					if (!strcmp("emupath", kv->key))
						emummc_path = kv->val;
				}
			}

			if (emummc_path && !emummc_set_path(emummc_path))
			{
				EPRINTF("emupath is wrong!");
				goto wrong_emupath;
			}

			if (cfg_sec && !payload_path)
				check_sept(cfg_sec);

			if (!cfg_sec)
			{
				free(ments);
				sd_end();
				return;
			}

			free(ments);
		}
		else
			EPRINTF("Could not open 'bootloader/hekate_ipl.ini'.\nMake sure it exists!");
	}

	if (!cfg_sec)
	{
		gfx_puts("\nUsing default launch configuration...\n");
		gfx_puts("\nPress POWER to Continue.\nPress VOL to go to the menu.");

		u32 btn = btn_wait();
		if (!(btn & BTN_POWER))
			goto out;
	}

	if (payload_path)
	{
		launch_payload(payload_path, false);
		EPRINTF("Failed to launch payload.");
		free(payload_path);
	}
	else if (!hos_launch(cfg_sec))
	{
wrong_emupath:
		EPRINTF("Failed to launch HOS.");
		if (emummc_path)
		{
			sd_mount();
			emummc_load_cfg();
		}
	}

out:
	sd_end();

	h_cfg.emummc_force_disable = false;

	btn_wait();
}

#define NYX_VER_OFF 0x9C

void nyx_load_run()
{
	sd_mount();

	u8* nyx = sd_file_read("argon/sys/argon-nx-gui.bin", NULL);
	if (!nyx)
		return;
	sd_end();
	
	// Set hekate version used to boot Nyx.
	nyx_str->version = ipl_ver.version - 0x303030; // Convert ASCII to numbers.

	// Set SD card initialization info.
	nyx_str->info.magic = NYX_NEW_INFO;
	nyx_str->info.sd_init = sd_get_mode();
	u16 *sd_errors = sd_get_error_count();
	for (u32 i = 0; i < 3; i++)
		nyx_str->info.sd_errors[i] = sd_errors[i];

	volatile reloc_meta_t *reloc = (reloc_meta_t *)(IPL_LOAD_ADDR + RELOC_META_OFF);
	memcpy((u8 *)nyx_str->hekate, (u8 *)reloc->start, reloc->end - reloc->start);

	void (*nyx_ptr)() = (void *)nyx;

	bpmp_mmu_disable();
	bpmp_clk_rate_set(BPMP_CLK_NORMAL);
	minerva_periodic_training();

	// Some cards (Sandisk U1), do not like a fast power cycle. Wait min 100ms.
	sdmmc_storage_init_wait_sd();

	(*nyx_ptr)();
}

//Autolaunch Payload wenn autolaunch.bin in argon Ordner sonst GUI laden oder mit halten Vol- autolaunch abbrechen
static void _auto_launch_firmware()
{
	if (f_stat("argon/autolaunch.bin", NULL))
	{
		goto out;
	}

	else
	{
		char* payload_path = NULL;
		u32 btn = 0;
		// Wait before booting. If VOL- is pressed go into bootloader menu.
		if (!h_cfg.sept_run && !(b_cfg.boot_cfg & BOOT_CFG_FROM_LAUNCH))
		{
			btn = btn_wait_timeout_single(h_cfg.bootwait * 1000, BTN_VOL_DOWN | BTN_SINGLE);

			if (btn & BTN_VOL_DOWN)
				goto out;
		}

		payload_path = "argon/autolaunch.bin";

		if (payload_path)
		{
			launch_payload(payload_path, false);
			goto out;
		}
	}

out:
	nyx_load_run();
}

static void _splash_laden()
{
        //Splash laden
        u8* BOOTLOGO = NULL;

        struct _bmp_data
        {
                u32 size;
                u32 size_x;
                u32 size_y;
                u32 offset;
                u32 pos_x;
                u32 pos_y;
        };

        struct _bmp_data bmpData;

        u8* bitmap = NULL;

        bitmap = (u8*)sd_file_read("argon/splash.bmp", NULL);//Bootlogo Custom

        if (bitmap)
        {
                // Get values manually to avoid unaligned access.
                bmpData.size = bitmap[2] | bitmap[3] << 8 |
                        bitmap[4] << 16 | bitmap[5] << 24;
                bmpData.offset = bitmap[10] | bitmap[11] << 8 |
                        bitmap[12] << 16 | bitmap[13] << 24;
                bmpData.size_x = bitmap[18] | bitmap[19] << 8 |
                        bitmap[20] << 16 | bitmap[21] << 24;
                bmpData.size_y = bitmap[22] | bitmap[23] << 8 |
                        bitmap[24] << 16 | bitmap[25] << 24;
                // Sanity check.
                if (bitmap[0] == 'B' &&
                        bitmap[1] == 'M' &&
                        bitmap[28] == 32 && // Only 32 bit BMPs allowed.
                        bmpData.size_x <= 720 &&
                        bmpData.size_y <= 1280)
                {
                        if ((bmpData.size - bmpData.offset) <= 0x400000)
                        {
                                // Avoid unaligned access from BM 2-byte MAGIC and remove header.
                                BOOTLOGO = (u8*)malloc(0x400000);
                                memcpy(BOOTLOGO, bitmap + bmpData.offset, bmpData.size - bmpData.offset);
                                free(bitmap);
                                // Center logo if res < 720x1280.
                                bmpData.pos_x = (720 - bmpData.size_x) >> 1;
                                bmpData.pos_y = (1280 - bmpData.size_y) >> 1;
                                // Get background color from 1st pixel.
                                if (bmpData.size_x < 720 || bmpData.size_y < 1280)
                                        gfx_clear_color(*(u32*)BOOTLOGO);

                                gfx_render_bmp_argb((u32*)BOOTLOGO, bmpData.size_x, bmpData.size_y,
                                        bmpData.pos_x, bmpData.pos_y);
                        }
                }
                else
                        free(bitmap);
        }

        free(BOOTLOGO);
        display_backlight_brightness(100, 1000);//default
}

static void _patched_rcm_protection()
{
	sdmmc_storage_t storage;
	sdmmc_t sdmmc;

	if (!h_cfg.rcm_patched || hw_get_chip_id() == GP_HIDREV_MAJOR_T210B01)
		return;

	// Check if AutoRCM is enabled and protect from a permanent brick.
	if (!sdmmc_storage_init_mmc(&storage, &sdmmc, SDMMC_BUS_WIDTH_8, SDHCI_TIMING_MMC_HS400))
		return;

	u8 *buf = (u8 *)malloc(0x200);
	sdmmc_storage_set_mmc_partition(&storage, EMMC_BOOT0);

	u32 sector;
	u8 corr_mod0, mod1;

	// Get the correct RSA modulus byte masks.
	nx_emmc_get_autorcm_masks(&corr_mod0, &mod1);

	// Iterate BCTs.
	for (u32 i = 0; i < 4; i++)
	{
		sector = 1 + (32 * i); // 0x4000 bct + 0x200 offset.
		sdmmc_storage_read(&storage, sector, 1, buf);

		// Check if 2nd byte of modulus is correct.
		if (buf[0x11] != mod1)
			continue;

		// If AutoRCM is enabled, disable it.
		if (buf[0x10] != corr_mod0)
		{
			buf[0x10] = corr_mod0;

			sdmmc_storage_write(&storage, sector, 1, buf);
		}
	}

	free(buf);
	sdmmc_storage_end(&storage);
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
		h_cfg.errors |= ERR_EXCEPTION;

	//! FIXME: Find a better way to identify if that scratch has proper data.
	if (0 && PMC(APBDEV_PMC_SCRATCH37) & PMC_SCRATCH37_KERNEL_PANIC_FLAG)
	{
		// Set error and clear flag.
		h_cfg.errors |= ERR_L4T_KERNEL;
		PMC(APBDEV_PMC_SCRATCH37) &= ~PMC_SCRATCH37_KERNEL_PANIC_FLAG;
	}

	if (h_cfg.errors)
	{
		gfx_clear_grey(0x1B);
		gfx_con_setpos(0, 0);
		display_backlight_brightness(150, 1000);

		if (h_cfg.errors & ERR_SD_BOOT_EN)
			WPRINTF("Failed to mount SD!\n");

		if (h_cfg.errors & ERR_LIBSYS_LP0)
			WPRINTF("Missing LP0 (sleep mode) lib!\n");
		if (h_cfg.errors & ERR_LIBSYS_MTC)
			WPRINTF("Missing or old Minerva lib!\n");

		if (h_cfg.errors & (ERR_LIBSYS_LP0 | ERR_LIBSYS_MTC))
			WPRINTF("\nUpdate bootloader folder!\n\n");

		if (h_cfg.errors & ERR_EXCEPTION)
		{
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
			*excp_enabled = 0;
		}

		if (0 && h_cfg.errors & ERR_L4T_KERNEL)
		{
			WPRINTF("Panic occurred while running L4T.\n");
			if (!sd_save_to_file((void *)PSTORE_ADDR, PSTORE_SZ, "L4T_panic.bin"))
				WPRINTF("PSTORE saved to L4T_panic.bin\n");
		}

		WPRINTF("Press any key...");

		msleep(1000);
		btn_wait();
	}
}

static void _check_low_battery()
{
	if (fuse_read_hw_state() == FUSE_NX_HW_STATE_DEV)
		goto out;

	int enough_battery;
	int batt_volt = 0;
	int charge_status = 0;

	bq24193_get_property(BQ24193_ChargeStatus, &charge_status);
	max17050_get_property(MAX17050_AvgVCELL, &batt_volt);

	enough_battery = charge_status ? 3250 : 3000;

	if (batt_volt > enough_battery || !batt_volt)
		goto out;

	// Prepare battery icon resources.
	u8 *battery_res = malloc(ALIGN(SZ_BATTERY_EMPTY, 0x1000));
	blz_uncompress_srcdest(BATTERY_EMPTY_BLZ, SZ_BATTERY_EMPTY_BLZ, battery_res, SZ_BATTERY_EMPTY);

	u8 *battery_icon = malloc(0x95A);  // 21x38x3
	u8 *charging_icon = malloc(0x2F4); // 21x12x3
	u8 *no_charging_icon = calloc(0x2F4, 1);

	memcpy(charging_icon, battery_res, 0x2F4);
	memcpy(battery_icon, battery_res + 0x2F4, 0x95A);

	u32 battery_icon_y_pos = 1280 - 16 - Y_BATTERY_EMPTY_BATT;
	u32 charging_icon_y_pos = 1280 - 16 - Y_BATTERY_EMPTY_BATT - 12 - Y_BATTERY_EMPTY_CHRG;
	free(battery_res);

	charge_status = !charge_status;

	u32 timer = 0;
	bool screen_on = false;
	while (true)
	{
		bpmp_msleep(250);

		// Refresh battery stats.
		int current_charge_status = 0;
		bq24193_get_property(BQ24193_ChargeStatus, &current_charge_status);
		max17050_get_property(MAX17050_AvgVCELL, &batt_volt);
		enough_battery = current_charge_status ? 3250 : 3000;

		if (batt_volt > enough_battery)
			break;

		// Refresh charging icon.
		if (screen_on && (charge_status != current_charge_status))
		{
			if (current_charge_status)
				gfx_set_rect_rgb(charging_icon, X_BATTERY_EMPTY, Y_BATTERY_EMPTY_CHRG, 16, charging_icon_y_pos);
			else
				gfx_set_rect_rgb(no_charging_icon, X_BATTERY_EMPTY, Y_BATTERY_EMPTY_CHRG, 16, charging_icon_y_pos);
		}

		// Check if it's time to turn off display.
		if (screen_on && timer < get_tmr_ms())
		{
			if (!current_charge_status)
			{
				max77620_low_battery_monitor_config(true);
				power_set_state(POWER_OFF_RESET);
			}

			display_end();
			screen_on = false;
		}

		// Check if charging status changed or Power button was pressed.
		if ((charge_status != current_charge_status) || (btn_wait_timeout_single(0, BTN_POWER) & BTN_POWER))
		{
			if (!screen_on)
			{
				display_init();
				u32 *fb = display_init_framebuffer_pitch();
				gfx_init_ctxt(fb, 720, 1280, 720);

				gfx_set_rect_rgb(battery_icon, X_BATTERY_EMPTY, Y_BATTERY_EMPTY_BATT, 16, battery_icon_y_pos);
				if (current_charge_status)
					gfx_set_rect_rgb(charging_icon, X_BATTERY_EMPTY, Y_BATTERY_EMPTY_CHRG, 16, charging_icon_y_pos);
				else
					gfx_set_rect_rgb(no_charging_icon, X_BATTERY_EMPTY, Y_BATTERY_EMPTY_CHRG, 16, charging_icon_y_pos);

				display_backlight_pwm_init();
				display_backlight_brightness(100, 1000);

				screen_on = true;
			}

			timer = get_tmr_ms() + 15000;
		}

		// Check if forcefully continuing.
		if (btn_read_vol() == (BTN_VOL_UP | BTN_VOL_DOWN))
			break;

		charge_status = current_charge_status;
	}

	display_end();

	free(battery_icon);
	free(charging_icon);
	free(no_charging_icon);

out:
	// Re enable Low Battery Monitor shutdown.
	max77620_low_battery_monitor_config(true);
}

void ipl_reload()
{
	hw_reinit_workaround(false, 0);

	// Reload hekate.
	void (*ipl_ptr)() = (void *)IPL_LOAD_ADDR;
	(*ipl_ptr)();
}

extern void pivot_stack(u32 stack_top);

void ipl_main()
{
	// Do initial HW configuration. This is compatible with consecutive reruns without a reset.
	hw_init();

	// Pivot the stack so we have enough space.
	pivot_stack(IPL_STACK_TOP);

	// Tegra/Horizon configuration goes to 0x80000000+, package2 goes to 0xA9800000, we place our heap in between.
	heap_init(IPL_HEAP_START);

#ifdef DEBUG_UART_PORT
	uart_send(DEBUG_UART_PORT, (u8 *)"hekate: Hello!\r\n", 16);
	uart_wait_idle(DEBUG_UART_PORT, UART_TX_IDLE);
#endif

	// Check if battery is enough.
	_check_low_battery();

	// Set bootloader's default configuration.
	set_default_configuration();

	// Mount SD Card.
	h_cfg.errors |= !sd_mount() ? ERR_SD_BOOT_EN : 0;

	// Save sdram lp0 config.
	void *sdram_params =
		hw_get_chip_id() == GP_HIDREV_MAJOR_T210 ? sdram_get_params_patched() : sdram_get_params_t210b01();
	if (!ianos_loader("argon/sys/lp0.bso", DRAM_LIB, sdram_params))//Ianos und Minerva zum emunsw ordner
		h_cfg.errors |= ERR_LIBSYS_LP0;

	// Train DRAM and switch to max frequency.
	if (minerva_init()) //!TODO: Add Tegra210B01 support to minerva.
		h_cfg.errors |= ERR_LIBSYS_MTC;

	display_init();

	u32 *fb = display_init_framebuffer_pitch();
	gfx_init_ctxt(fb, 720, 1280, 720);

	gfx_con_init();

	display_backlight_pwm_init();

	// Overclock BPMP.
	bpmp_clk_rate_set(BPMP_CLK_DEFAULT_BOOST);

	// Check if we had a panic while in CFW.
	secmon_exo_check_panic();

	// Check if RCM is patched and protect from a possible brick.
	_patched_rcm_protection();

	// Load emuMMC configuration from SD.
	emummc_load_cfg();

	// Show exception, library errors and L4T kernel panics.
	_show_errors();

	// Load splash screen if available
	_splash_laden();

	// Load saved configuration and auto boot if enabled.
	_auto_launch_firmware();

	// Failed to launch Nyx, unmount SD Card.
	sd_end();

	minerva_change_freq(FREQ_800);

	// Halt BPMP if we managed to get out of execution.
	while (true)
		bpmp_halt();
}

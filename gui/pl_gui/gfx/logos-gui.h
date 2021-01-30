/*
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

#ifndef _LOGOS_GUI_H_
#define _LOGOS_GUI_H_

#include <memory_map.h>

#include <libs/lv_conf.h>
#include <libs/lvgl/lv_draw/lv_draw_img.h>
#include <utils/types.h>

#define HEKATE_LOGO

const u8 touch_cursor_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0xff, 0xff, 0x02, 0x00, 0xff, 0xff, 0x02, 0x00, 0xff, 0xff, 0x01, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0xaa, 0xff, 0x03, 0x00, 0xaa, 0xaa, 0x06, 0x00, 0xaa, 0xd4, 0x06, 0x00, 0x9f, 0xbf, 0x08, 0x00, 0x9f, 0xbf, 0x08, 0x00, 0xbf, 0xff, 0x04, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x02, 0x00, 0x9f, 0x9f, 0x08, 0x00, 0x8c, 0xa5, 0x14, 0x00, 0x91, 0xa3, 0x0e, 0x00, 0x8d, 0xa9, 0x09, 0x00, 0x8b, 0xb9, 0x16, 0x00, 0x88, 0xbb, 0x0f, 0x00, 0x99, 0xcc, 0x05, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbf, 0xbf, 0x04, 0x00, 0x9c, 0x9c, 0x0d, 0x00, 0x99, 0x99, 0x0f, 0x6e, 0x9e, 0x00, 0x96, 0x65, 0x9e, 0x34, 0x97, 0x00, 0x86, 0xbb, 0x13, 0x00, 0x97, 0xbc, 0x1b, 0x00, 0x99, 0xbb, 0x0f, 0x00, 0xbf, 0xff, 0x04, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcc, 0x99, 0x05, 0x00, 0x96, 0x87, 0x11, 0x00, 0x8d, 0x8d, 0x1b, 0x78, 0xa6, 0x0f, 0xc7, 0xbe, 0xc1, 0x00, 0xff, 0x3d, 0xa1, 0x63, 0x85, 0x00, 0x8c, 0xbf, 0x14, 0x00, 0x9a, 0xc8, 0x1c, 0x00, 0xaa, 0xbf, 0x0c, 0x00, 0xbf, 0xff, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0xaa, 0x7f, 0x06, 0x00, 0x9d, 0x79, 0x15, 0x00, 0x93, 0x78, 0x26, 0x69, 0xab, 0x47, 0xc9, 0x6b, 0xa9, 0x00, 0xf9, 0xb5, 0xd4, 0x00, 0xff, 0x22, 0x9b, 0x84, 0x6e, 0x00, 0x96, 0xd2, 0x11, 0x00, 0xa6, 0xcd, 0x1a, 0x00, 0xa2, 0xd0, 0x0b, 0x00, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x01, 0x00, 0xb6, 0x48, 0x07, 0x00, 0x9b, 0x63, 0x17, 0x00, 0x92, 0x6c, 0x28, 0x7d, 0xb4, 0x0e, 0xda, 0x25, 0x99, 0x62, 0xab, 0x38, 0xaa, 0x57, 0xa3, 0xd3, 0xd9, 0x00, 0xff, 0x00, 0x9b, 0x92, 0x52, 0x00, 0xa5, 0xd2, 0x11, 0x00, 0xad, 0xcb, 0x19, 0x00, 0xa9, 0xe2, 0x09, 0x00, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x00, 0xb6, 0x24, 0x07, 0x00, 0xa2, 0x51, 0x16, 0x00, 0x9b, 0x55, 0x24, 0x85, 0xb0, 0x00, 0xdb, 0x79, 0xb9, 0x79, 0xce, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xb9, 0x00, 0xd0, 0xda, 0xd7, 0x00, 0xff, 0x00, 0x9c, 0xa1, 0x39, 0x00, 0xae, 0xe4, 0x13, 0x00, 0xad, 0xd0, 0x16, 0x00, 0xbf, 0xdf, 0x08, 0x00, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x24, 0xb6, 0x00, 0x07, 0x00, 0xa9, 0x30, 0x15, 0x00, 0x9c, 0x39, 0x1f, 0x8d, 0xad, 0x00, 0xd7, 0x72, 0xb0, 0x49, 0xc6, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xae, 0xc6, 0x00, 0xe9, 0xc6, 0xcf, 0x00, 0xee, 0x00, 0x9b, 0xae, 0x29, 0x00, 0xb1, 0xdd, 0x17, 0x00, 0xb2, 0xcc, 0x14, 0x00, 0xb6, 0xda, 0x07, 0x00, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x24, 0xb6, 0x00, 0x07, 0x0c, 0xa9, 0x00, 0x15, 0x00, 0xa3, 0x0a, 0x19, 0x91, 0xae, 0x00, 0xd0, 0x5e, 0x9d, 0x00, 0xb5, 0xfd, 0xfe, 0xfd, 0xf8, 0xfd, 0xff, 0xff, 0xfd, 0xfd, 0xfe, 0xfe, 0xf5, 0xd6, 0xd2, 0x00, 0xff, 0xa8, 0xc7, 0x39, 0xd2, 0x00, 0xa3, 0xb7, 0x19, 0x00, 0xb0, 0xe1, 0x1a, 0x00, 0xa9, 0xd4, 0x12, 0x00, 0xcc, 0xff, 0x05, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xb6, 0x00, 0x07, 0x24, 0xa9, 0x00, 0x15, 0x00, 0x9f, 0x00, 0x18, 0x99, 0xad, 0x00, 0xd1, 0x67, 0x9d, 0x00, 0xb1, 0xfe, 0xfe, 0xfe, 0xf8, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xfe, 0xfd, 0xf2, 0xef, 0xda, 0x00, 0xff, 0x7a, 0xba, 0x59, 0xab, 0x00, 0xa5, 0xc3, 0x11, 0x00, 0xb6, 0xda, 0x1c, 0x00, 0xaa, 0xcc, 0x0f, 0x00, 0xbf, 0xff, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0xda, 0x00, 0x07, 0x3c, 0xa9, 0x00, 0x15, 0x1f, 0xaa, 0x00, 0x18, 0x9e, 0xae, 0x00, 0xd1, 0x70, 0x9d, 0x00, 0xaf, 0xfe, 0xfe, 0xfe, 0xf4, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfb, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xe0, 0x00, 0xff, 0x4e, 0xb0, 0x6f, 0x85, 0x00, 0xb0, 0xd7, 0x0d, 0x00, 0xad, 0xd1, 0x1c, 0x00, 0xaa, 0xd4, 0x0c, 0x00, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0xda, 0x00, 0x07, 0x48, 0xb6, 0x00, 0x15, 0x35, 0xaa, 0x00, 0x18, 0xa3, 0xb0, 0x00, 0xd2, 0x78, 0x9e, 0x00, 0xae, 0xfd, 0xfe, 0xfe, 0xea, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfa, 0xe0, 0x00, 0xff, 0x1e, 0xa8, 0x7f, 0x64, 0x00, 0xaf, 0xcf, 0x10, 0x00, 0xb3, 0xcf, 0x1b, 0x00, 0xb2, 0xcc, 0x0a, 0x00, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x6d, 0xb6, 0x00, 0x07, 0x61, 0xb6, 0x00, 0x15, 0x47, 0xad, 0x00, 0x19, 0xa5, 0xae, 0x00, 0xd4, 0x7b, 0x9e, 0x00, 0xae, 0xfd, 0xfe, 0xfe, 0xf4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xed, 0xdc, 0x00, 0xff, 0x00, 0xa6, 0x90, 0x45, 0x00, 0xb8, 0xd4, 0x12, 0x00, 0xb4, 0xc9, 0x18, 0x00, 0xbf, 0xdf, 0x08, 0x00, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x91, 0xda, 0x00, 0x07, 0x6d, 0xb6, 0x00, 0x15, 0x58, 0xb0, 0x00, 0x1a, 0xa8, 0xae, 0x00, 0xd4, 0x80, 0x9f, 0x00, 0xad, 0xfd, 0xfe, 0xfd, 0xe2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xb6, 0xc6, 0x00, 0xe1, 0xd1, 0xd4, 0x00, 0xf7, 0x00, 0xa4, 0x9e, 0x2d, 0x00, 0xbc, 0xd2, 0x17, 0x00, 0xb6, 0xc2, 0x15, 0x00, 0xb6, 0xda, 0x07, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x91, 0xda, 0x00, 0x07, 0x6d, 0xb6, 0x00, 0x15, 0x5b, 0xad, 0x00, 0x1c, 0xad, 0xaf, 0x00, 0xd9, 0x85, 0xa0, 0x00, 0xac, 0xfe, 0xfe, 0xfe, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xd9, 0xd4, 0x00, 0xff, 0xaf, 0xca, 0x2c, 0xd9, 0x00, 0xa4, 0xa4, 0x1f, 0x00, 0xb3, 0xcf, 0x1b, 0x00, 0xae, 0xc9, 0x13, 0x00, 0xaa, 0xd4, 0x06, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x9f, 0xbf, 0x00, 0x08, 0x73, 0xad, 0x00, 0x16, 0x66, 0xaa, 0x00, 0x1e, 0xaf, 0xad, 0x00, 0xdb, 0x88, 0x9e, 0x00, 0xac, 0xfe, 0xfe, 0xfe, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xfd, 0xfe, 0xfe, 0xfe, 0xfe, 0xec, 0xe2, 0x00, 0xff, 0x7d, 0xbe, 0x50, 0xb5, 0x00, 0x9e, 0xa7, 0x1d, 0x00, 0xac, 0xc5, 0x1f, 0x00, 0xa5, 0xc3, 0x11, 0x00, 0xcc, 0xcc, 0x05, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x9f, 0xbf, 0x00, 0x08, 0x7f, 0xad, 0x00, 0x16, 0x6f, 0xa7, 0x00, 0x20, 0xb0, 0xab, 0x00, 0xdf, 0x89, 0x9d, 0x00, 0xac, 0xff, 0xff, 0xfd, 0xf0, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xed, 0xee, 0x02, 0xff, 0x4b, 0xb0, 0x5a, 0x98, 0x00, 0x9e, 0xa7, 0x1d, 0x00, 0xa4, 0xb4, 0x1f, 0x00, 0x9c, 0xb0, 0x0d, 0x00, 0xaa, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x9f, 0xbf, 0x00, 0x08, 0x85, 0xa6, 0x00, 0x17, 0x75, 0xa5, 0x00, 0x25, 0xac, 0xa5, 0x00, 0xe6, 0x8c, 0x9b, 0x00, 0xb0, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0xfd, 0xfc, 0xf6, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0x46, 0xb7, 0x44, 0xab, 0xe1, 0xf3, 0x48, 0xff, 0x23, 0xa1, 0x57, 0x7b, 0x00, 0x9b, 0xa6, 0x17, 0x00, 0x9b, 0xa6, 0x17, 0x00, 0xaa, 0xaa, 0x06, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x8d, 0xa9, 0x00, 0x09, 0x7f, 0x9f, 0x00, 0x18, 0x7c, 0x9b, 0x00, 0x29, 0xa6, 0x9e, 0x00, 0xea, 0x8f, 0x9a, 0x00, 0xb9, 0xfe, 0xfe, 0xfe, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfc, 0xfe, 0xfe, 0xfe, 0xfc, 0x81, 0xa8, 0x00, 0xce, 0x7f, 0x95, 0x00, 0xff, 0x79, 0x9d, 0x00, 0xfd, 0x6f, 0x99, 0x00, 0xfd, 0x5b, 0x9c, 0x00, 0xed, 0x39, 0x91, 0x00, 0xde, 0x94, 0xb7, 0x00, 0xff, 0xd6, 0xda, 0x00, 0xff, 0x22, 0x96, 0x44, 0x69, 0x00, 0xa9, 0xa9, 0x09, 0x00, 0x9f, 0x9f, 0x08, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x8d, 0xa9, 0x00, 0x09, 0x74, 0x94, 0x00, 0x18, 0x7c, 0x9c, 0x00, 0x31, 0xa7, 0x9f, 0x00, 0xf0, 0x92, 0x9d, 0x00, 0xbf, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xb5, 0xb1, 0x00, 0xff, 0x94, 0xc3, 0x00, 0xbb, 0x98, 0xc8, 0x00, 0xcd, 0xc5, 0xdc, 0x00, 0xff, 0x70, 0xae, 0x00, 0xe0, 0x5f, 0xa1, 0x00, 0xd3, 0x66, 0x9c, 0x00, 0xdf, 0x67, 0x9c, 0x00, 0xe1, 0x56, 0x90, 0x00, 0xd7, 0x9c, 0xab, 0x00, 0xf9, 0x54, 0x9b, 0x00, 0x7b, 0x00, 0xaa, 0xaa, 0x03, 0x00, 0x9f, 0x9f, 0x08, 0x00, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x7f, 0x99, 0x00, 0x0a, 0x7f, 0x93, 0x00, 0x1a, 0x7f, 0x99, 0x00, 0x3a, 0xa8, 0xa1, 0x00, 0xf5, 0x92, 0x9e, 0x00, 0xc3, 0xfe, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xa6, 0xa7, 0x00, 0xff, 0xdc, 0xdf, 0x00, 0xff, 0xc8, 0xe5, 0x00, 0xe6, 0x76, 0xba, 0x00, 0xb3, 0xf1, 0xfc, 0x3d, 0xff, 0x82, 0xc6, 0x00, 0xb8, 0x06, 0xa5, 0x00, 0x4a, 0x00, 0x9e, 0x00, 0x47, 0x00, 0x97, 0x00, 0x3b, 0x00, 0x92, 0x0e, 0x36, 0x00, 0x8f, 0x2b, 0x29, 0x00, 0x97, 0x5e, 0x1b, 0x00, 0x9b, 0x71, 0x12, 0x00, 0x99, 0x99, 0x05, 0x00, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x99, 0xb2, 0x00, 0x0a, 0x84, 0x97, 0x00, 0x1b, 0x87, 0xa1, 0x00, 0x44, 0xa3, 0x9e, 0x00, 0xfa, 0x95, 0xa7, 0x00, 0xba, 0x89, 0xa0, 0x00, 0xa0, 0xbd, 0xb6, 0x00, 0xff, 0xaa, 0xc0, 0x00, 0xc7, 0xb0, 0xcd, 0x00, 0xc2, 0xf0, 0xf5, 0x00, 0xff, 0x7d, 0xbf, 0x00, 0xb1, 0xc9, 0xe8, 0x00, 0xee, 0xd5, 0xe5, 0x00, 0xfc, 0x4c, 0xb5, 0x00, 0x64, 0x39, 0xb4, 0x00, 0x3a, 0x13, 0x9f, 0x00, 0x28, 0x00, 0x98, 0x00, 0x19, 0x00, 0x99, 0x22, 0x0f, 0x00, 0x9c, 0x4e, 0x0d, 0x00, 0x8d, 0x54, 0x09, 0x00, 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0x99, 0xb2, 0x00, 0x0a, 0x89, 0x9c, 0x00, 0x1a, 0x8a, 0x9f, 0x00, 0x48, 0xb3, 0xb0, 0x00, 0xef, 0xa3, 0xa7, 0x00, 0xfb, 0xb6, 0xaf, 0x00, 0xff, 0xb9, 0xbe, 0x00, 0xd1, 0x82, 0xb3, 0x00, 0x4e, 0x91, 0xbc, 0x00, 0x83, 0xf2, 0xee, 0x00, 0xff, 0xb1, 0xd8, 0x00, 0xc9, 0x97, 0xcc, 0x00, 0xb6, 0xf9, 0xf2, 0x1e, 0xff, 0x8c, 0xc4, 0x00, 0x95, 0x5b, 0xc8, 0x00, 0x2a, 0x4a, 0xbd, 0x00, 0x1f, 0x24, 0xb6, 0x00, 0x0e, 0x00, 0x91, 0x00, 0x07, 0x00, 0xaa, 0x00, 0x03, 0x00, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0xa9, 0xa9, 0x00, 0x09, 0x8c, 0xa5, 0x00, 0x14, 0x8d, 0x9b, 0x00, 0x48, 0xc9, 0xbf, 0x00, 0xf6, 0xda, 0xcb, 0x00, 0xff, 0xaf, 0xae, 0x00, 0xce, 0x83, 0xaa, 0x00, 0x42, 0x97, 0xbf, 0x00, 0x40, 0x98, 0xc3, 0x00, 0x52, 0xd7, 0xdc, 0x00, 0xe2, 0xe4, 0xec, 0x00, 0xff, 0x87, 0xc1, 0x00, 0x9e, 0xea, 0xf0, 0x00, 0xff, 0xc9, 0xd6, 0x00, 0xde, 0x6d, 0xc5, 0x00, 0x3a, 0x6a, 0xd5, 0x00, 0x1f, 0x55, 0xd4, 0x00, 0x0c, 0x7f, 0xff, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x06, 0x99, 0xaa, 0x00, 0x0f, 0x96, 0xa8, 0x00, 0x2c, 0xba, 0xa9, 0x00, 0xdc, 0xb7, 0xad, 0x00, 0xd2, 0x89, 0xa1, 0x00, 0x34, 0x90, 0xb3, 0x00, 0x25, 0x9f, 0xbf, 0x00, 0x30, 0xa8, 0xd0, 0x00, 0x2c, 0xba, 0xc9, 0x00, 0x9c, 0xfa, 0xee, 0x00, 0xff, 0x9f, 0xca, 0x00, 0xaa, 0xbe, 0xde, 0x00, 0xcc, 0xf6, 0xeb, 0x00, 0xff, 0x85, 0xbf, 0x00, 0x71, 0x77, 0xd7, 0x00, 0x20, 0x6b, 0xc9, 0x00, 0x13, 0x7f, 0xff, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xff, 0x00, 0x03, 0x91, 0xa3, 0x00, 0x0e, 0x94, 0xaa, 0x00, 0x18, 0x97, 0x9d, 0x00, 0x2a, 0x8e, 0xa5, 0x00, 0x22, 0x8e, 0xad, 0x00, 0x19, 0x93, 0xb0, 0x00, 0x1a, 0xaa, 0xc9, 0x00, 0x18, 0xb6, 0xda, 0x00, 0x23, 0xaa, 0xc8, 0x00, 0x5d, 0xef, 0xe5, 0x00, 0xff, 0xd0, 0xe0, 0x00, 0xdd, 0x95, 0xc5, 0x00, 0xa0, 0xfa, 0xf4, 0x25, 0xff, 0xb5, 0xcc, 0x00, 0xbb, 0x73, 0xc8, 0x00, 0x2a, 0x76, 0xc8, 0x00, 0x1c, 0x5f, 0xdf, 0x00, 0x08, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x02, 0xcc, 0xcc, 0x00, 0x05, 0x94, 0xaa, 0x00, 0x0c, 0x91, 0xb6, 0x00, 0x07, 0x99, 0xb2, 0x00, 0x0a, 0x91, 0xb6, 0x00, 0x0e, 0x9f, 0xbf, 0x00, 0x08, 0xaa, 0xd4, 0x00, 0x0c, 0xb8, 0xd3, 0x00, 0x1d, 0xa8, 0xce, 0x00, 0x2f, 0xd2, 0xd4, 0x00, 0xcd, 0xf6, 0xf0, 0x00, 0xff, 0x92, 0xc0, 0x00, 0x92, 0xda, 0xed, 0x00, 0xe1, 0xee, 0xe8, 0x00, 0xff, 0x7f, 0xba, 0x00, 0x52, 0x7b, 0xca, 0x00, 0x1d, 0x6a, 0xbf, 0x00, 0x0c, 0x7f, 0xff, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0xaa, 0xff, 0x00, 0x03, 0xbf, 0xbf, 0x00, 0x04, 0xbf, 0xbf, 0x00, 0x04, 0xaa, 0xff, 0x00, 0x03, 0xff, 0xff, 0x00, 0x02, 0xcc, 0xcc, 0x00, 0x05, 0xb6, 0xce, 0x00, 0x15, 0xaf, 0xd7, 0x00, 0x20, 0xb2, 0xc2, 0x00, 0x86, 0xff, 0xf1, 0x00, 0xff, 0xb7, 0xca, 0x00, 0xce, 0x8d, 0xa9, 0x00, 0xd5, 0xea, 0xde, 0x00, 0xff, 0xa8, 0xba, 0x00, 0x9c, 0x7f, 0xbf, 0x00, 0x14, 0x75, 0xc4, 0x00, 0x0d, 0x7f, 0xff, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x02, 0xbf, 0xd4, 0x00, 0x0c, 0xb8, 0xd3, 0x00, 0x1d, 0xa1, 0xbb, 0x00, 0x44, 0xe1, 0xda, 0x00, 0xec, 0xdf, 0xd7, 0x00, 0xff, 0xc2, 0xbe, 0x00, 0xff, 0xc7, 0xbc, 0x00, 0xff, 0x9a, 0xa7, 0x00, 0x95, 0x88, 0xbb, 0x00, 0x0f, 0x73, 0xb9, 0x00, 0x0b, 0x7f, 0xff, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x01, 0xb6, 0xda, 0x00, 0x07, 0xb1, 0xc7, 0x00, 0x17, 0xa4, 0xc5, 0x00, 0x1f, 0xa7, 0xae, 0x00, 0x92, 0xc5, 0xba, 0x00, 0xfd, 0xbc, 0xc1, 0x00, 0xbc, 0x99, 0xb1, 0x00, 0x6c, 0x90, 0xb6, 0x00, 0x2e, 0x8e, 0xb7, 0x00, 0x19, 0x8d, 0xc6, 0x00, 0x09, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x02, 0xbf, 0xbf, 0x00, 0x08, 0xaa, 0xcc, 0x00, 0x0f, 0xa6, 0xbc, 0x00, 0x17, 0x9a, 0xb1, 0x00, 0x2b, 0x99, 0xb2, 0x00, 0x14, 0xaa, 0xd4, 0x00, 0x06, 0x9c, 0xc4, 0x00, 0x0d, 0x99, 0xb2, 0x00, 0x0a, 0xaa, 0xff, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

lv_img_dsc_t touch_cursor = {
	.header.always_zero = 0,
	.header.w = 33,
	.header.h = 33,
	.data_size = 1089 * LV_IMG_PX_SIZE_ALPHA_BYTE,
	.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
	.data = touch_cursor_map,
};

#ifdef HEKATE_LOGO

lv_img_dsc_t hekate_logo = {
	.header.always_zero = 0,
	.header.w = 193,
	.header.h = 76,
	.data_size = 14668 * LV_IMG_PX_SIZE_ALPHA_BYTE,
	.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
	.data = (const uint8_t *)(NYX_RES_ADDR + 0x1D900),
};

lv_img_dsc_t ctcaer_logo = {
	.header.always_zero = 0,
	.header.w = 147,
	.header.h = 76,
	.data_size = 11172 * LV_IMG_PX_SIZE_ALPHA_BYTE,
	.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
	.data = (const uint8_t *)(NYX_RES_ADDR + 0x2BF00),
};

#endif

#endif
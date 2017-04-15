/*
 * Copyright (C) 2015-2017 secunet Security Networks AG
 * Copyright (C) 2017 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef GFX_EDID_H
#define GFX_EDID_H

#include <inttypes.h>
#include <stdbool.h>
#include <string.h> // memcpy

#include "gfx.h"

#define REVISION			19
#define INPUT				20
#define DESCRIPTOR_1			54
#define INPUT_DIGITAL			(1 << 7)
#define INPUT_DIGITAL_DEPTH_SHIFT	4
#define INPUT_DIGITAL_DEPTH_MASK	(7 << 4)
#define INPUT_DIGITAL_DEPTH_UNDEF	(0 << 4)
#define INPUT_DIGITAL_DEPTH_RESERVED	(7 << 4)

bool edid_Checksum_Valid(uint8_t Raw_EDID[]);
bool edid_Sanitize(uint8_t Raw_EDID[]);
bool edid_Compatible_Display(uint8_t Raw_EDID[], Display_Type Display);
bool edid_Has_Preferred_Mode(uint8_t Raw_EDID[]);
Mode_Type edid_Preferred_Mode(uint8_t Raw_EDID[]);

#endif

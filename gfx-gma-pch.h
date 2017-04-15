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

#ifndef GFX_GMA_PCH_H
#define GFX_GMA_PCH_H

typedef enum {
	FDI_A,
	FDI_B,
	FDI_C
} FDI_Port_Type;

//  common to all PCH outputs
#define PCH_TRANSCODER_SELECT_SHIFT		29
#define PCH_TRANSCODER_SELECT_MASK		(3 << 29)

typedef Word32 PCH_TRANSCODER_SELECT_Array[3];

const PCH_TRANSCODER_SELECT_Array PCH_TRANSCODER_SELECT = {
	0 << PCH_TRANSCODER_SELECT_SHIFT,
	1 << PCH_TRANSCODER_SELECT_SHIFT,
	2 << PCH_TRANSCODER_SELECT_SHIFT
};

#endif

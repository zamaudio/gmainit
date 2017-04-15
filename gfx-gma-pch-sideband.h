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

#ifndef GFX_GMA_PCH_SIDEBAND_H
#define GFX_GMA_PCH_SIDEBAND_H

typedef enum {
	SBI_ICLK,
	SBI_MPHY
} Destination_Type;

typedef enum {
	SBI_SSCDIVINTPHASE6,
	SBI_SSCCTL6,
	SBI_SSCAUXDIV
} Register_Type;

void Read(Destination_Type Dest, Register_Type Register, Word32 *Value);
void Write(Destination_Type Dest, Register_Type Register, Word32 Value);
void Unset_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask);
void Set_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask);
void Unset_And_Set_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask_Unset, Word32 Mask_Set);

#endif

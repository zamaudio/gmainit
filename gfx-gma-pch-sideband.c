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

#include "gfx-gma-registers.h"
#include "gfx-gma-pch-sideband.h"

#define SBI_CTL_DEST_ICLK		(0 << 16)
#define SBI_CTL_DEST_MPHY		(1 << 16)
#define SBI_CTL_OP_IORD			(2 << 8)
#define SBI_CTL_OP_IOWR			(3 << 8)
#define SBI_CTL_OP_CRRD			(6 << 8)
#define SBI_CTL_OP_CRWR			(7 << 8)
#define SBI_RESPONSE_FAIL		(1 << 1)
#define SBI_BUSY			(1 << 0)

#define SBI_SSCDIVINTPHASE6		0x06000000
#define SBI_SSCCTL6			0x060c0000
#define SBI_SSCAUXDIV			0x06100000

typedef Word32 Register_Array[];

const Register_Array Register_Addr = {
	SBI_SSCDIVINTPHASE6,
	SBI_SSCCTL6,
	SBI_SSCAUXDIV
};

void Read(Destination_Type Dest, Register_Type Register, Word32 *Value)
{
	Registers.Wait_Unset_Mask(Registers.SBI_CTL_STAT, SBI_BUSY);
	Registers.Write(Registers.SBI_ADDR, Register_Addr[Register]);
	if(Dest == SBI_ICLK) {
		Registers.Write(Registers.SBI_CTL_STAT,
				SBI_CTL_DEST_ICLK |
				SBI_CTL_OP_CRRD |
				SBI_BUSY);
	} else {
		Registers.Write(Registers.SBI_CTL_STAT,
				SBI_CTL_DEST_MPHY |
				SBI_CTL_OP_IORD |
				SBI_BUSY);
	}
	Registers.Wait_Unset_Mask(Registers.SBI_CTL_STAT, SBI_BUSY);
	Registers.Read(Registers.SBI_DATA, Value);
}

void Write(Destination_Type Dest, Register_Type Register, Word32 Value)
{
	Registers.Wait_Unset_Mask(Registers.SBI_CTL_STAT, SBI_BUSY);
	Registers.Write(Registers.SBI_ADDR, Register_Addr[Register]);
	Registers.Write(Registers.SBI_DATA, Value);
	if(Dest == SBI_ICLK) {
		Registers.Write(Registers.SBI_CTL_STAT,
				SBI_CTL_DEST_ICLK |
				SBI_CTL_OP_CRWR |
				SBI_BUSY);
	} else {
		Registers.Write(Registers.SBI_CTL_STAT,
				SBI_CTL_DEST_MPHY |
				SBI_CTL_OP_IOWR |
				SBI_BUSY);
	}
	Registers.Wait_Unset_Mask(Registers.SBI_CTL_STAT, SBI_BUSY);
}

void Unset_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask)
{
	Word32 Value;
	Read(Dest, Register, Value);
	Write(Dest, Register, Value & ~Mask);
}

void Set_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask)
{
	Word32 Value;
	Read(Dest, Register, Value);
	Write(Dest, Register, Value | Mask);
}

void Unset_And_Set_Mask(Destination_Type Dest, Register_Type Register, Word32 Mask_Unset, Word32 Mask_Set)
{
	Word32 Value;
	Read(Dest, Register, Value);
	Write(Dest, Register, (Value & ~Mask_Unset) | Mask_Set);
}

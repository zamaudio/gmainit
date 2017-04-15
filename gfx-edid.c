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

#include <inttypes.h>
#include <stdbool.h>
#include <string.h> // memcpy

#include "gfx.h"
#include "gfx-edid.h"

const uint8_t Header_Pattern[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

static bool Checksum_Valid(uint8_t Raw_EDID[])
{
	uint8_t Sum = 0x00;
	for(int I = 0; I < 128; I++) {
		Sum = Sum + Raw_EDID[I];
	}
	return Sum == 0x00;
}

static uint32_t Header_Score(uint8_t Raw_EDID[])
{
	uint32_t Score = 0;
	for(int I = 0; I < 8; I++) {
		if(Raw_EDID[I] == Header_Pattern[I]) {
			Score = Score + 1;
		}
	}
	return Score;
}

static uint16_t Read_LE16(uint8_t Raw_EDID[], uint8_t Offset)
{
	return ((uint16_t)(Raw_EDID[Offset + 1]) << 8) | (uint16_t)(Raw_EDID[Offset]);
}

bool edid_Valid(uint8_t Raw_EDID[])
{
	return (Header_Score(Raw_EDID) == 8) && Checksum_Valid(Raw_EDID);
}

bool edid_Sanitize(uint8_t Raw_EDID[])
{
	const uint32_t Score = Header_Score(Raw_EDID);

	if(Score == 6 || Score == 7) {
		memcpy(Raw_EDID, Header_Pattern, 8);
	}
	return Valid(Raw_EDID);
}

bool edid_Compatible_Display(uint8_t Raw_EDID[], Display_Type Display)
{
	return (Display == VGA) && ((Raw_EDID[INPUT] & INPUT_DIGITAL) == 0);
}

bool edid_Has_Preferred_Mode(uint8_t Raw_EDID[])
{
/* Int64(Read_LE16(Raw_EDID, DESCRIPTOR_1)) * 10_000 in Frequency_Type && */
	return (
	(Raw_EDID[DESCRIPTOR_1 + 2]!=0 || (Raw_EDID[DESCRIPTOR_1 + 4] & 0xf0)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 8]!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0xc0)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 9]!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x30)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 3]!=0 || (Raw_EDID[DESCRIPTOR_1 + 4] & 0x0f)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 5]!=0 || (Raw_EDID[DESCRIPTOR_1 + 7] & 0xf0)!=0) &&
	((Raw_EDID[DESCRIPTOR_1 + 10] & 0xf0)!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x0c)!=0) &&
	((Raw_EDID[DESCRIPTOR_1 + 10] & 0x0f)!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x03)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 6]!=0 || (Raw_EDID[DESCRIPTOR_1 + 7] & 0x0f)!=0));
}

Mode_Type edid_Preferred_Mode(uint8_t Raw_EDID[])
{
	const uint8_t Base = DESCRIPTOR_1;
	Mode_Type Mode;

	uint16_t Read_12(uint8_t Lower_8, uint8_t Upper_4, uint32_t Shift)
	{
		return (uint16_t)(Raw_EDID[Lower_8]) |
			(((uint16_t)(Raw_EDID[Upper_4]) << Shift) & 0x0f00);
	}

	uint16_t Read_10(uint8_t Lower_8, uint8_t Upper_2, uint32_t Shift)
	{
		return (uint16_t)(Raw_EDID[Lower_8]) |
			(((uint16_t)(Raw_EDID[Upper_2]) << Shift) & 0x0300);
	}

	uint8_t Read_6(uint8_t Lower_4, uint32_t Lower_Shift, uint8_t Upper_2, uint32_t Upper_Shift)
	{
		return (((uint8_t)(Raw_EDID[Lower_4]) >> Lower_Shift) & 0x0f) |
			(((uint8_t)(Raw_EDID[Upper_2]) << Upper_Shift) & 0x30);
	}

	if((Raw_EDID[REVISION] < 4) || ((Raw_EDID[INPUT] & INPUT_DIGITAL) == 0) ||
			((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) == INPUT_DIGITAL_DEPTH_UNDEF) ||
			((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) == INPUT_DIGITAL_DEPTH_RESERVED)) {
		Mode.BPC = Auto_BPC;
	} else {
		Mode.BPC = 4 + 2 * (uint64_t)((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) >> INPUT_DIGITAL_DEPTH_SHIFT);
	}
	
	Mode.Dotclock = (uint64_t)(Read_LE16(Raw_EDID, Base)) * 10000;
	Mode.H_Visible = (uint16_t)(Read_12(Base + 2, Base + 4, 4));
	Mode.H_Sync_Begin = (uint16_t)(Read_10(Base + 8, Base + 11, 2));
	Mode.H_Sync_End = (uint16_t)(Read_10(Base + 9, Base + 11, 4));
	Mode.H_Total = (uint16_t)(Read_12(Base + 3, Base + 4, 8));
	Mode.V_Visible = (uint16_t)(Read_12(Base + 5, Base + 7, 4));
	Mode.V_Sync_Begin = (uint16_t)(Read_6(Base + 10, 4, Base + 11, 2));
	Mode.V_Sync_End = (uint16_t)(Read_6(Base + 10, 0, Base + 11, 4));
	Mode.V_Total = (uint16_t)(Read_12(Base + 6, Base + 7, 8));
	Mode.H_Sync_Active_High = (Raw_EDID[Base + 17] & 0x02)!=0;
	Mode.V_Sync_Active_High = (Raw_EDID[Base + 17] & 0x04)!=0;

	//  Calculate absolute values from EDID relative values.
	Mode.H_Sync_Begin = Mode.H_Visible + Mode.H_Sync_Begin;
	Mode.H_Sync_End = Mode.H_Sync_Begin + Mode.H_Sync_End;
	Mode.H_Total = Mode.H_Visible + Mode.H_Total;
	Mode.V_Sync_Begin = Mode.V_Visible + Mode.V_Sync_Begin;
	Mode.V_Sync_End = Mode.V_Sync_Begin + Mode.V_Sync_End;
	Mode.V_Total = Mode.V_Visible + Mode.V_Total;
	return Mode;
}

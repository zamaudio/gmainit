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

const Byte Header_Pattern[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};

Boolean Checksum_Valid(Byte Raw_EDID[])
{
	Byte Sum = 0x00;
	for(int I = 0; I < 128; I++) {
		Sum = Sum + Raw_EDID[I];
	}
	return Sum == 0x00;
}

Natural Header_Score(Byte Raw_EDID[])
{
	Natural Score = 0;
	for(int I = 0; I < 8; I++) {
		if(Raw_EDID[I] == Header_Pattern[I]) {
			Score = Score + 1;
		}
	}
	return Score;
}

Boolean Valid(Byte Raw_EDID[])
{
	return (Header_Score(Raw_EDID) == 8) && Checksum_Valid(Raw_EDID);
}

Boolean Sanitize(Byte *Raw_EDID)
{
	const Natural Score = Header_Score(Raw_EDID);

	if(Score == 6 || Score == 7) {
		memcpy(Raw_EDID, Header_Pattern, 8);
	}
	return Valid(Raw_EDID);
}

// --------------------------------------------------------------------------
#define REVISION			19
#define INPUT				20
#define INPUT_DIGITAL			(1 << 7)
#define INPUT_DIGITAL_DEPTH_SHIFT	4
#define INPUT_DIGITAL_DEPTH_MASK	(7 << 4)
#define INPUT_DIGITAL_DEPTH_UNDEF	(0 << 4)
#define INPUT_DIGITAL_DEPTH_RESERVED	(7 << 4)
// --------------------------------------------------------------------------

Boolean Compatible_Display(Byte Raw_EDID[], enum Display_Type Display)
{
	return (Display == VGA) && ((Raw_EDID[INPUT] & INPUT_DIGITAL) == 0);
}

Word16 Read_LE16(Byte Raw_EDID[], Byte Offset)
{
	return (Word16(Raw_EDID[Offset + 1]) << 8) | Word16(Raw_EDID[Offset]);
}

Boolean Has_Preferred_Mode(Raw_EDID_Data Raw_EDID)
{
  /* Int64(Read_LE16(Raw_EDID, DESCRIPTOR_1)) * 10_000 in Frequency_Type && */
  return (Raw_EDID[DESCRIPTOR_1 + 2]!=0 || (Raw_EDID[DESCRIPTOR_1 + 4] & 0xf0)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 8]!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0xc0)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 9]!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x30)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 3]!=0 || (Raw_EDID[DESCRIPTOR_1 + 4] & 0x0f)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 5]!=0 || (Raw_EDID[DESCRIPTOR_1 + 7] & 0xf0)!=0) &&
	((Raw_EDID[DESCRIPTOR_1 + 10] & 0xf0)!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x0c)!=0) &&
	((Raw_EDID[DESCRIPTOR_1 + 10] & 0x0f)!=0 || (Raw_EDID[DESCRIPTOR_1 + 11] & 0x03)!=0) &&
	(Raw_EDID[DESCRIPTOR_1 + 6]!=0 || (Raw_EDID[DESCRIPTOR_1 + 7] & 0x0f)!=0);
}

Mode_Type Preferred_Mode(Byte Raw_EDID[])
{
	const Byte Base = DESCRIPTOR_1;
	struct Mode_Type Mode;

	Word16 Read_12(Byte Lower_8, Byte Upper_4, Natural Shift)
	{
		return Word16(Raw_EDID[Lower_8]) |
			((Word16(Raw_EDID[Upper_4]) << Shift) & 0x0f00);
	}

	Word16 Read_10(Byte Lower_8, Byte Upper_2, Natural Shift)
	{
		return Word16(Raw_EDID[Lower_8]) |
			((Word16(Raw_EDID[Upper_2]) << Shift) & 0x0300);
	}

	Word8 Read_6(Byte Lower_4, Natural Lower_Shift, Byte Upper_2, Natural Upper_Shift)
	{
		return ((Word8(Raw_EDID[Lower_4]) >> Lower_Shift) & 0x0f) |
			((Word8(Raw_EDID[Upper_2]) << Upper_Shift) & 0x30);
	}

	if((Raw_EDID[REVISION] < 4) || ((Raw_EDID[INPUT] & INPUT_DIGITAL) == 0) ||
			((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) == INPUT_DIGITAL_DEPTH_UNDEF) ||
			((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) == INPUT_DIGITAL_DEPTH_RESERVED)) {
		Mode.BPC = Auto_BPC;
	} else {
		Mode.BPC = 4 + 2 * Pos64((Raw_EDID[INPUT] & INPUT_DIGITAL_DEPTH_MASK) >> INPUT_DIGITAL_DEPTH_SHIFT);
	}
	
	Mode.Dotclock = Pos64(Read_LE16(Raw_EDID, Base)) * 10000;
	Mode.H_Visible = Pos16(Read_12(Base + 2, Base + 4, 4));
	Mode.H_Sync_Begin = Pos16(Read_10(Base + 8, Base + 11, 2));
	Mode.H_Sync_End = Pos16(Read_10(Base + 9, Base + 11, 4));
	Mode.H_Total = Pos16(Read_12(Base + 3, Base + 4, 8));
	Mode.V_Visible = Pos16(Read_12(Base + 5, Base + 7, 4));
	Mode.V_Sync_Begin = Pos16(Read_6(Base + 10, 4, Base + 11, 2));
	Mode.V_Sync_End = Pos16(Read_6(Base + 10, 0, Base + 11, 4));
	Mode.V_Total = Pos16(Read_12(Base + 6, Base + 7, 8));
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

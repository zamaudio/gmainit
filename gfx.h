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

#ifndef GFX_H
#define GFX_H

#include <inttypes.h>
#include <stdbool.h>

//  implementation only supports 4800p for now ;-)
typedef uint32_t Width_Type;
typedef uint32_t Height_Type;

#define Auto_BPC				5

typedef int64_t BPC_Type;

typedef struct t_Framebuffer_Type {
	Width_Type Width;
	Height_Type Height;
	BPC_Type BPC;
	Width_Type Stride;
	uint32_t Offset;
} Framebuffer_Type;

const Framebuffer_Type Default_FB = {
	1,
	1,
	8,
	1,
	0
};

typedef uint64_t Frequency_Type;

typedef enum {
	DP_Lane_Count_1,
	DP_Lane_Count_2,
	DP_Lane_Count_4
} DP_Lane_Count;

typedef uint64_t DP_Lane_Count_Type;
typedef DP_Lane_Count_Type DP_Lane_Count_Integers[3];

const DP_Lane_Count_Integers Lane_Count_As_Integer = {
	1,
	2,
	4
};

typedef enum
{
	DP_Bandwidth_1_62 = 6,
	DP_Bandwidth_2_7 = 10,
	DP_Bandwidth_5_4 = 20
} DP_Bandwidth;

typedef uint64_t DP_Symbol_Rate_Type;
typedef DP_Symbol_Rate_Type DP_Symbol_Rate_Array[21];

const DP_Symbol_Rate_Array DP_Symbol_Rate = {
	0,0,0,0,0,0,
	/* DP_Bandwidth_1_62 => */ 162000000,
	0,0,0,
	/* DP_Bandwidth_2_7 => */ 270000000,
	0,0,0,0,0,0,0,0,0,
	/* DP_Bandwidth_5_4 => */ 540000000
};

typedef struct t_DP_Caps {
	uint8_t Rev;
	DP_Bandwidth Max_Link_Rate;
	DP_Lane_Count Max_Lane_Count;
	bool TPS3_Supported;
	bool Enhanced_Framing;
	bool No_Aux_Handshake;
	uint8_t Aux_RD_Interval;
} DP_Caps;

typedef struct t_DP_Link {
	DP_Caps Receiver_Caps;
	DP_Lane_Count Lane_Count;
	DP_Bandwidth Bandwidth;
	bool Enhanced_Framing;
	bool Opportunistic_Training;
} DP_Link;

const DP_Link Default_DP = {
	{
		0x00,
		DP_Bandwidth_1_62,
		DP_Lane_Count_1,
		false,
		false,
		false,
		0x00
	},
	DP_Lane_Count_1,
	DP_Bandwidth_1_62,
	false,
	false
};

typedef enum {
	None,
	LVDS,
	DP,
	HDMI,
	VGA
} Display_Type;

typedef Display_Type Internal_Type;

typedef struct t_Mode_Type {
	Frequency_Type Dotclock;
	uint16_t H_Visible;
	uint16_t H_Sync_Begin;
	uint16_t H_Sync_End;
	uint16_t H_Total;
	uint16_t V_Visible;
	uint16_t V_Sync_Begin;
	uint16_t V_Sync_End;
	uint16_t V_Total;
	bool H_Sync_Active_High;
	bool V_Sync_Active_High;
	BPC_Type BPC;
} Mode_Type;

//  modeline constants
//  Dotclock is calculated using: Refresh_Rate * H_Total * V_Total
const Mode_Type M2560x1600_60 = { 60 * (2720 * 1646), 2560, 2608, 2640, 2720, 1600, 1603, 1609, 1646, true, true, Auto_BPC };
const Mode_Type M2560x1440_60 = { 60 * (2720 * 1481), 2560, 2608, 2640, 2720, 1440, 1443, 1448, 1481, true, false, Auto_BPC };
const Mode_Type M1920x1200_60 = { 60 * (2080 * 1235), 1920, 1968, 2000, 2080, 1200, 1203, 1209, 1235, false, false, Auto_BPC };
const Mode_Type M1920x1080_60 = { 60 * (2185 * 1135), 1920, 2008, 2052, 2185, 1080, 1084, 1089, 1135, false, false, Auto_BPC };
const Mode_Type M1680x1050_60 = { 60 * (2256 * 1087), 1680, 1784, 1968, 2256, 1050, 1051, 1054, 1087, false, true, Auto_BPC };
const Mode_Type M1600x1200_60 = { 60 * (2160 * 1250), 1600, 1664, 1856, 2160, 1200, 1201, 1204, 1250, true, true, Auto_BPC };
const Mode_Type M1600x900_60 = { 60 * (2010 * 912), 1600, 1664, 1706, 2010, 900, 903, 906, 912, false, false, Auto_BPC };
const Mode_Type M1440x900_60 = { 60 * (1834 * 920), 1440, 1488, 1520, 1834, 900, 903, 909, 920, false, false, Auto_BPC };
const Mode_Type M1366x768_60 = { 60 * (1446 * 788), 1366, 1414, 1446, 1466, 768, 769, 773, 788, false, false, Auto_BPC };
const Mode_Type M1280x1024_60 = { 60 * (1712 * 1063), 1280, 1368, 1496, 1712, 1024, 1027, 1034, 1063, false, true, Auto_BPC };
const Mode_Type M1024x768_60 = { 60 * (1344 * 806), 1024, 1048, 1184, 1344, 768, 771, 777, 806, false, false, Auto_BPC };
const Mode_Type Invalid_Mode = { 0, 1, 1, 1, 1, 1, 1, 1, 1, false, false, Auto_BPC };

#endif

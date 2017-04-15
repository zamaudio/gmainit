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

typedef enum {
	VS_Level_0,
	VS_Level_1,
	VS_Level_2,
	VS_Level_3
} DP_Voltage_Swing;

typedef enum {
	Emph_Level_0,
	Emph_Level_1,
	Emph_Level_2,
	Emph_Level_3
} DP_Pre_Emph;

typedef struct {
	DP_Voltage_Swing Voltage_Swing;
	DP_Pre_Emph Pre_Emph;
} Train_Set;

typedef enum {
	TP_1,
	TP_2,
	TP_3,
	TP_Idle,
	TP_None
} Training_Pattern;

typedef struct {
	Boolean CR_Done;
	Boolean Channel_EQ_Done;
	Boolean Symbol_Locked;
	Boolean Reserved;
} Lane_Status;

typedef struct Lane_Status Lanes_Status[4];

typedef struct {
	DP_Voltage_Swing Voltage_Swing;
	DP_Pre_Emph Pre_Emph;
} Adjust_Request;

typedef struct Adjust_Request Adjust_Requests_Array[4];

typedef struct {
	union {
		Lanes_Status;
		Boolean Lanes[16];
	};
	Boolean Interlane_Align_Done;
	union {
		Adjust_Requests_Array;
		Boolean Adjust_Requests[16];
	};
} Link_Status;

#define M_N_Max ((1 << 24) - 1)

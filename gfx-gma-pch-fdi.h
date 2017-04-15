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

#ifndef GFX_GMA_PCH_FDI_H
#define GFX_GMA_PCH_FDI_H

typedef enum {
	TP_1,
	TP_2,
	TP_Idle,
	TP_None
} Training_Pattern;

void Pre_Train(PCH.FDI_Port_Type Port, Port_Config Port_Cfg);
Boolean Train(PCH.FDI_Port_Type Port, Training_Pattern TP);
void Auto_Train(PCH.FDI_Port_Type Port);
void Enable_EC(PCH.FDI_Port_Type Port);

typedef enum {
	Rx_Off,
	Lanes_Off,
	Clock_Off
} Off_Type;

void Off(PCH.FDI_Port_Type Port, Off_Type OT);

#endif

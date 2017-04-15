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

#ifndef GFX_GMA_DISPLAY_PROBING_H
#define GFX_GMA_DISPLAY_PROBING_H

typedef int Port_List_Range;
typedef Port_Type Port_List[8];
const Port_List All_Ports = { DP1, DP2, DP3, HDMI1, HDMI2, HDMI3, Analog, Internal };
void Scan_Ports(Pipe_Configs *Configs, Port_List Ports, Pipe_Index Max_Pipe, Boolean Keep_Power);

#endif

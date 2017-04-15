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

#include <stdbool.h>
#include <inttypes.h>
#include "gfx-gma.h"

bool Preferred_Link_Setting(Port_Config *Port_Cfg)
{
	bool Success;
	const DP_Port DP_Port = DP_A;
	
	if(Port_Cfg->Display == DP) {
		if(Port_Cfg->Port == DIGI_A) {
			if(CONFIG_Use_PP_VDD_Override) {
				panel_VDD_Override();
			} else {
				panel_On();
			}
		}
		Success = dpinfo_Read_Caps(Port_Cfg->DP, DP_Port);
		if(Success) {
			dpinfo_Preferred_Link_Setting(Port_Cfg->DP, Port_Cfg->Mode);
		}
	} else {
		Success = true;
	}
	return Success;
}

bool Next_Link_Setting(Port_Config *Port_Cfg)
{
	bool Success;

	if(Port_Cfg->Display == DP) {
		Success = dpinfo_Next_Link_Setting(Port_Cfg->DP, Port_Cfg->Mode);
	} else {
		Success = false;
	}
	return Success;
}

uint8_t Default_BPC(Port_Config Port_Cfg)
{
	if( (Port_Cfg.Port == DIGI_A) ||
			(Port_Cfg.Is_FDI && (Port_Cfg.PCH_Port == PCH_LVDS)) ) {
		return 6;
	} else {
		return 8;
	}
}

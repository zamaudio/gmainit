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

Boolean Preferred_Link_Setting(struct Port_Config *Port_Cfg)
{
	Boolean Success;
	const GMA.DP_Port DP_Port = DP_A;
	
	if(Port_Cfg.Display == DP) {
		if(Port_Cfg.Port == DIGI_A) {
			if(GMA.Config.Use_PP_VDD_Override) {
				Panel.VDD_Override();
			} else {
				Panel.On();
			}
		}
		Success = DP_Info.Read_Caps(Port_Cfg.DP, DP_Port);
		if(Success) {
			DP_Info.Preferred_Link_Setting(Port_Cfg.DP, Port_Cfg.Mode);
		}
	} else {
		Success = True;
	}
	return Success;
}

Boolean Next_Link_Setting(Port_Config *Port_Cfg)
{
	Boolean Success;

	if(Port_Cfg.Display == DP) {
		Success = DP_Info.Next_Link_Setting(Port_Cfg.DP, Port_Cfg.Mode);
	} else {
		Success = False;
	}
	return Success;
}

Byte Default_BPC(Port_Config Port_Cfg)
{
	if( (Port_Cfg.Port == DIGI_A) ||
			(Port_Cfg.Is_FDI && (Port_Cfg.PCH_Port == PCH_LVDS)) ) {
		return 6;
	} else {
		return 8;
	}
}

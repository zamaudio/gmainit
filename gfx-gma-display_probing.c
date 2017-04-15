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

#include "gfx-edid.h"
#include "gfx-i2c.h"
#include "gfx-gma-i2c.h"
#include "gfx-gma-panel.h"
#include "gfx-gma-display_probing.h"

bool Port_Configured(Pipe_Configs *Configs, Port_Type Port)
{
	return (Configs[Primary]->Port == Port) ||
		(Configs[Secondary]->Port == Port) ||
		(Configs[Tertiary]->Port == Port);
}

//  DP and HDMI share physical pins.
Port_Type Sibling_Port(Port_Type Port)
{
	switch(Port) {
	case HDMI1:
		return DP1;
	case HDMI2:
		return DP2;
	case HDMI3:
		return DP3;
	case DP1:
		return HDMI1;
	case DP2:
		return HDMI2;
	case DP3:
		return HDMI3;
	default:
		return Disabled;
	}
}

bool Has_Sibling_Port(Port_Type Port)
{
	return (Sibling_Port(Port) != Disabled);
}

bool Read_EDID(uint8_t Raw_EDID[], Active_Port_Type Port)
{
	bool Success = false;
	uint8_t Raw_EDID_Length = 128;

	for( int I = 1 ; I <= 2 ; I++) {
		if(confighelpers_To_Display_Type(Port) == DP) {
			//  May need power to read edid
			Pipe_Configs Temp_Configs = Cur_Configs;
			Temp_Configs[Primary].Port = Port;
			powerandclocks_Power_Up(Cur_Configs, Temp_Configs);
			const DP_Port DP_Port = DP_A;
			Success = dpauxch_I2C_Read(DP_Port, 0x50, Raw_EDID_Length, Raw_EDID);
		} else {
			uint32_t Prt;
			if(Port == Analog) {
				Prt = CONFIG_Analog_I2C_Port;
			} else {
				Prt = confighelpers_To_PCH_Port(Port);
			}
			Success = i2c_I2C_Read(Prt, 0x50, Raw_EDID_Length, Raw_EDID);
		}
		//  don't retry if reading itself failed
		if(!Success)
			break;

		Success = edid_Sanitize(Raw_EDID);
		if(Success)
			break;
	}
	return Success;
}

bool Probe_Port(Pipe_Config *Pipe_Cfg, Active_Port_Type Port)
{
	bool Success;
	uint8_t Raw_EDID[128] = { 0 };
	
	Success = CONFIG_Valid_Port(Port);
	if(Success) {
		if(Port == Internal) {
			panel_Wait_On();
		}
		Success = Read_EDID(Raw_EDID, Port);
	}
	if(Success && (edid_Compatible_Display(Raw_EDID, confighelpers_To_Display_Type(Port)) && edid_Has_Preferred_Mode(Raw_EDID))) {
		Pipe_Cfg->Port = Port;
		Pipe_Cfg->Mode = edid_Preferred_Mode(Raw_EDID);
		if(Has_Sibling_Port(Port)) {
			// Probe sibling port too and bail out if something is detected.
			// This is a precaution for adapters that expose the pins of a
			// port for both HDMI/DVI and DP (like some ThinkPad docks). A
			// user might have attached both by accident and there are ru-
			// mors of displays that got fried by applying the wrong signal.
			bool Have_Sibling_EDID = Read_EDID(Raw_EDID, Sibling_Port(Port));
			if(Have_Sibling_EDID) {
				Pipe_Cfg->Port = Disabled;
				Success = false;
			}
		}
	} else {
		Success = false;
	}
	return Success;
}

void Scan_Ports(Pipe_Configs *Configs, Port_List Ports, Pipe_Index Max_Pipe, bool Keep_Power)
{
	bool Probe_Internal = false;
	Port_List_Range Port_Idx = 0;
	bool Success;
	Configs[0]->Port = Disabled;
	Configs[0]->Mode = Invalid_Mode;
	Configs[0]->Framebuffer = Default_FB;
	Configs[1]->Port = Disabled;
	Configs[1]->Mode = Invalid_Mode;
	Configs[1]->Framebuffer = Default_FB;
	Configs[2]->Port = Disabled;
	Configs[2]->Mode = Invalid_Mode;
	Configs[2]->Framebuffer = Default_FB;
	//  Turn panel on early to probe other ports during the power on delay.
	for( int Idx = 0; Idx < 8 ; Idx++) {
		if(Ports[Idx] == Internal) {
			panel_On(false);
			Probe_Internal = true;
			break;
		}
	}

	for( int Pipe = 0; Pipe < 3; Pipe++) {
		while( Ports[Port_Idx]!=Disabled) {
			if(!Port_Configured(Configs, Ports[Port_Idx]) && (!Has_Sibling_Port(Ports[Port_Idx]) || !Port_Configured(Configs, Sibling_Port(Ports[Port_Idx])))) {
				Success = Probe_Port(Configs[Pipe], Ports[Port_Idx]);
			} else {
				Success = false;
			}
			Port_Idx++;
			break;
		}
	}

	//  Restore power settings
	if(!Keep_Power) {
		powerandclocks_Power_Set_To(Cur_Configs);
	}
	//  Turn panel power off if probing failed.
	if(Probe_Internal && !Port_Configured(Configs, Internal)) {
		panel_Off();
	}
}

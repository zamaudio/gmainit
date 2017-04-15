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
#include "gfx-gma-registers.h"
#include "gfx-gma-panel.h"
#include "gfx-gma-port_detect.h"
#include "gfx-gma-connectors.h"
#include "gfx-gma-connector_info.h"
#include "gfx-gma-pipe_setup.h"
#include "gfx-gma.h"

typedef char Port_Name[8];

typedef Port_Name Port_Name_Array[9];

const Port_Name_Array Port_Names = {
	"Disabled",
	"Internal",
	"DP1     ",
	"DP2     ",
	"DP3     ",
	"HDMI1   ",
	"HDMI2   ",
	"HDMI3   ",
	"Analog  "
};

#define Display_Controller Pipe_Setup

typedef PLLs.T PLLs_Type[3];
typedef bool HPD_Type[9];
typedef Time.T HPD_Delay_Type[9];

PLLs_Type Allocated_PLLs;
HPD_Delay_Type HPD_Delay;
HPD_Type Wait_For_HPD;

bool Initialized = false;

// --------------------------------------------------------------------------
#define PCH_RAWCLK_FREQ_MASK 0x3ff

uint32_t PCH_RAWCLK_FREQUENCY(Frequency_Type Freq)
{
	return (uint32_t)(Freq / 1000000);
}
// --------------------------------------------------------------------------

bool Enable_Output(Pipe_Index Pipe, Pipe_Config Pipe_Cfg)
{
	bool Success;
	Port_Config Port_Cfg;
	Success = Config_Helpers.Fill_Port_Config(Port_Cfg, Pipe, Pipe_Cfg.Port, Pipe_Cfg.Mode);
	if(Success) {
		Success = Config_Helpers.Validate_Config(Pipe_Cfg.Framebuffer, Port_Cfg, Pipe);
	}

	if(Success) {
		Success = Connector_Info.Preferred_Link_Setting(Port_Cfg);
	}
	//  loop over all possible DP-lane configurations
	//  (non-DP ports use a single fake configuration)
	while(Success) {
		Success = PLLs.Alloc(Port_Cfg, Allocated_PLLs[Pipe]);
		if(Success) {
			//  try each DP-lane configuration twice
			for( int Try = 1 ; Try <= 2 ; Try++) {
				//  Clear pending hot-plug events before every try
				Port_Detect.Clear_Hotplug_Detect(Pipe_Cfg.Port);
				Success = Connectors.Pre_On(Pipe, Port_Cfg, PLLs.Register_Value(Allocated_PLLs[Pipe]));
				if(Success) {
					Display_Controller.On(Pipe, Port_Cfg, Pipe_Cfg.Framebuffer);
					Success = Connectors.Post_On(Port_Cfg, PLLs.Register_Value(Allocated_PLLs[Pipe]));
					if(!Success) {
						Display_Controller.Off(Pipe);
						Connectors.Post_Off(Port_Cfg);
					}
				}
				break;
			}
			//  connection established => stop loop
			break;
			//  connection failed
			PLLs.Free(Allocated_PLLs[Pipe]);
		}
		Success = Connector_Info.Next_Link_Setting(Port_Cfg);
	}

	if(!Success) {
		Wait_For_HPD[Pipe_Cfg.Port] = true;
		if(Pipe_Cfg.Port == Internal) {
			Panel.Off();
		}
	}

	return Success;
}

void Disable_Output(Pipe_Index Pipe, Pipe_Config Pipe_Cfg)
{
	Port_Config Port_Cfg;
	bool Success;

	Success = Config_Helpers.Fill_Port_Config(Port_Cfg, Pipe, Pipe_Cfg.Port, Pipe_Cfg.Mode);
	if(Success) {
		Connectors.Pre_Off(Port_Cfg);
		Display_Controller.Off(Pipe);
		Connectors.Post_Off(Port_Cfg);
		PLLs.Free(Allocated_PLLs[Pipe]);
	}
}

void Update_Outputs(Pipe_Configs Configs)
{
	void Check_HPD(Active_Port_Type Port, bool *Detected)
	{
		const bool HPD_Delay_Over = Time.Timed_Out(HPD_Delay[Port]);
		if(HPD_Delay_Over) {
			Port_Detect.Hotplug_Detect(Port, Detected);
			HPD_Delay[Port] = Time.MS_From_Now(333);
		} else {
			*Detected = false;
		}
	}

	bool Power_Changed = false;
	Pipe_Configs Old_Configs;

	void Update_Power(void)
	{
		if(!Power_Changed) {
			Power_And_Clocks.Power_Up(Old_Configs, Configs);
			Power_Changed = true;
		}
	}
	Old_Configs = Cur_Configs;
	//  disable all pipes that changed or had a hot-plug event
	for(int Pipe = 0; Pipe < 3; Pipe++) {
		bool Unplug_Detected;
		
		if(Cur_Configs[Pipe].Port != Disabled) {
			Check_HPD(Cur_Configs[Pipe].Port, &Unplug_Detected);
			if((Cur_Configs[Pipe].Port != Configs[Pipe].Port) ||
					(Cur_Configs[Pipe].Mode != Configs[Pipe].Mode) ||
					Unplug_Detected) {
				Disable_Output(Pipe, Cur_Configs[Pipe]);
				Cur_Configs[Pipe].Port = Disabled;
				Update_Power();
			}
		}
	}

	//  enable all pipes that changed and should be active
	for( int Pipe = 0; Pipe < 3; Pipe++)
	{
		bool Success;

		if(Configs[Pipe].Port!=Disabled && (Cur_Configs[Pipe].Port!=Configs[Pipe].Port || Cur_Configs[Pipe].Mode!=Configs[Pipe].Mode)) {
			if(Wait_For_HPD[Configs[Pipe].Port]) {
				Success = Check_HPD(Configs[Pipe].Port);
				Wait_For_HPD[Configs[Pipe].Port] = !Success;
			} else {
				Success = true;
			}
			if(Success) {
				Update_Power();
				Success = Enable_Output(Pipe, Configs[Pipe]);
			}
			if(Success) {
				Cur_Configs[Pipe] = Configs[Pipe];
			}
		} else if(Configs[Pipe].Port!=Disabled && Cur_Configs[Pipe].Framebuffer!=Configs[Pipe].Framebuffer) {
			Display_Controller.Update_Offset(Pipe, Configs[Pipe].Framebuffer);
			Cur_Configs[Pipe] = Configs[Pipe];
		}
	}

	if(Power_Changed) {
		Power_And_Clocks.Power_Down(Old_Configs, Configs, Cur_Configs);
	}
}

// --------------------------------------------------------------------------
bool Initialize(uint64_t MMIO_Base, uint64_t Write_Delay, bool Clean_State)
{
	bool Success;
	const Time.T Now = Time.Now;

	bool Check_Platform(void)
	{
		uint32_t Audio_VID_DID;
		switch(Config.CPU) {
		case Haswell:
		case Broadwell:
		case Skylake:
			Registers.Read(AUD_VID_DID, &Audio_VID_DID);
			break;
		case Ironlake:
		case Sandybridge:
		case Ivybridge:
			Registers.Read(PCH_AUD_VID_DID, &Audio_VID_DID);
			break;
		}
		switch(Config.CPU) {
		case Sandybridge:
		case Ivybridge:
		case Haswell:
		case Broadwell:
		case Skylake:
			Success = (Audio_VID_DID == 0x80862800);
			break;
		case Ironlake:
			Success = (Audio_VID_DID == 0);
			break;
		}
		return Success;
	}

	Wait_For_HPD = false;
	HPD_Delay = Now;
	Allocated_PLLs = PLLs.Invalid;
	Cur_Configs[0].Port = Disabled;
	Cur_Configs[0].Framebuffer = Default_FB;
	Cur_Configs[0].Mode = Invalid_Mode;
	Cur_Configs[1].Port = Disabled;
	Cur_Configs[1].FrameBuffer = Default_FB;
	Cur_Configs[1].Mode = Invalid_Mode;
	Cur_Configs[2].Port = Disabled;
	Cur_Configs[2].Framebuffer = Default_FB;
	Cur_Configs[2].Mode = Invalid_Mode;
	
	Registers.Set_Register_Base((MMIO_Base != 0) ? MMIO_Base : Config.Default_MMIO_Base);
	PLLs.Initialize();
	Success = Check_Platform();
	if(!Success) {
		Initialized = false;
		return Success;
	}
	Panel.Setup_PP_Sequencer();
	Port_Detect.Initialize();
	if(Clean_State) {
		Power_And_Clocks.Pre_All_Off();
		Connectors.Pre_All_Off();
		Display_Controller.All_Off();
		Connectors.Post_All_Off();
		PLLs.All_Off();
		Power_And_Clocks.Post_All_Off();
	} else {
		//  According to PRMs, VGA plane is the only thing
		//  that's enabled by default after reset.
		Display_Controller.Legacy_VGA_Off();
	}
	// ------------------ Now restart from a clean state ---------------------
	Power_And_Clocks.Initialize();
	Registers.Unset_And_Set_Mask(PCH_RAWCLK_FREQ, PCH_RAWCLK_FREQ_MASK, PCH_RAWCLK_FREQUENCY(Config.Default_RawClk_Freq));
	Initialized = true;
	return Success;
}

// --------------------------------------------------------------------------
void Write_GTT(GTT_Range GTT_Page, GTT_Address_Type Device_Address, bool Valid)
{
	Registers.Write_GTT(GTT_Page, Device_Address, Valid);
}

void Setup_Default_GTT(Framebuffer_Type FB, uint32_t Phys_FB)
{
	const uint32_t FB_Size = FB.Stride * FB.Height * (uint32_t)(((FB.BPC * 4) / 8));
	GTT_Address_Type Phys_Addr = GTT_Address_Type(Phys_FB);
	for( int Idx = 0; Idx < (FB_Size + 4095) / 4096; Idx++) {
		Registers.Write_GTT(Idx, Phys_Addr, true);
		Phys_Addr = Phys_Addr + 4096;
	}
}

/*
 * Copyright (C) 2015-2016 secunet Security Networks AG
 * Copyright (C) 2016 Nico Huber <nico.h@gmx.de>
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

#include "gfx-gma-registers.h"
#include "gfx-gma-pch-transcoder.h"

typedef uint32_t DPLL_SEL_Array[3];

const DPLL_SEL_Array DPLL_SEL_TRANSCODER_x_DPLL_ENABLE = {
	1 << 3,
	1 << 7,
	1 << 11
};

const DPLL_SEL_Array DPLL_SEL_TRANSCODER_x_DPLL_SEL_MASK = {
	1 << 0,
	1 << 4,
	1 << 8
};

uint32_t DPLL_SEL_TRANSCODER_x_DPLL_SEL(FDI_Port_Type Port, uint32_t PLL)
{
	switch(Port) {
	case FDI_A:
		return (PLL << 0);
	case FDI_B:
		return (PLL << 4);
	case FDI_C:
		return (PLL << 8);
	}
}

#define TRANS_CONF_TRANSCODER_ENABLE		(1 << 31)
#define TRANS_CONF_TRANSCODER_STATE		(1 << 30)
#define TRANS_CHICKEN2_TIMING_OVERRIDE		(1 << 31)
#define TRANS_DP_CTL_OUTPUT_ENABLE		(1 << 31)
#define TRANS_DP_CTL_PORT_SELECT_MASK		(3 << 29)
#define TRANS_DP_CTL_PORT_SELECT_NONE		(3 << 29)
#define TRANS_DP_CTL_ENHANCED_FRAMING		(1 << 18)
#define TRANS_DP_CTL_VSYNC_ACTIVE_HIGH		(1 << 4)
#define TRANS_DP_CTL_HSYNC_ACTIVE_HIGH		(1 << 3)

typedef uint32_t TRANS_DP_CTL_PORT_SELECT_Array[8];
const TRANS_DP_CTL_PORT_SELECT_Array TRANS_DP_CTL_PORT_SELECT = {
	0,
	0,
	0,
	0,
	0,
	/* PCH_DP_B */ 0 << 29,
	/* PCH_DP_C */ 1 << 29,
	/* PCH_DP_D */ 2 << 29
};

uint32_t TRANS_DP_CTL_BPC(BPC_Type BPC)
{
	switch(BPC) {
	case 6:
		return 2 << 9;
	case 10:
		return 1 << 9;
	case 12:
		return 3 << 9;
	default:
		return 0;
	}
}

uint32_t TRANS_DATA_M_TU(uint32_t Transfer_Unit)
{
	return ((Transfer_Unit - 1) << 25);
}

typedef struct t_Transcoder_Registers {
	uint32_t HTOTAL;
	uint32_t HBLANK;
	uint32_t HSYNC;
	uint32_t VTOTAL;
	uint32_t VBLANK;
	uint32_t VSYNC;
	uint32_t CONF;
	uint32_t DP_CTL;
	uint32_t DATA_M;
	uint32_t DATA_N;
	uint32_t LINK_M;
	uint32_t LINK_N;
	uint32_t CHICKEN2;
} Transcoder_Registers;

typedef Transcoder_Registers Transcoder_Registers_Array[3];

const Transcoder_Registers_Array TRANS = {
	{
		TRANS_HTOTAL_A,
		TRANS_HBLANK_A,
		TRANS_HSYNC_A,
		TRANS_VTOTAL_A,
		TRANS_VBLANK_A,
		TRANS_VSYNC_A,
		TRANSACONF,
		TRANS_DP_CTL_A,
		TRANSA_DATA_M1,
		TRANSA_DATA_N1,
		TRANSA_DP_LINK_M1,
		TRANSA_DP_LINK_N1,
		TRANSA_CHICKEN2
	},
	{
		TRANS_HTOTAL_B,
		TRANS_HBLANK_B,
		TRANS_HSYNC_B,
		TRANS_VTOTAL_B,
		TRANS_VBLANK_B,
		TRANS_VSYNC_B,
		TRANSBCONF,
		TRANS_DP_CTL_B,
		TRANSB_DATA_M1,
		TRANSB_DATA_N1,
		TRANSB_DP_LINK_M1,
		TRANSB_DP_LINK_N1,
		TRANSB_CHICKEN2
	},
	{
		TRANS_HTOTAL_C,
		TRANS_HBLANK_C,
		TRANS_HSYNC_C,
		TRANS_VTOTAL_C,
		TRANS_VBLANK_C,
		TRANS_VSYNC_C,
		TRANSCCONF,
		TRANS_DP_CTL_C,
		TRANSC_DATA_M1,
		TRANSC_DATA_N1,
		TRANSC_DP_LINK_M1,
		TRANSC_DP_LINK_N1,
		TRANSC_CHICKEN2
	}
};

void On(Port_Config Port_Cfg, FDI_Port_Type Port, uint32_t PLL)
{
	const Mode_Type Mode = Port_Cfg.Mode;
	uint32_t Encode(uint16_t LSW, uint16_t MSW)
	{
		return ((uint32_t)(LSW) - 1) | (((uint32_t)(MSW) - 1) << 16);
	}
	
	if(CONFIG_Has_DPLL_SEL) {
		Registers.Unset_And_Set_Mask(PCH_DPLL_SEL,
				DPLL_SEL_TRANSCODER_x_DPLL_SEL_MASK[Port],
				DPLL_SEL_TRANSCODER_x_DPLL_ENABLE[Port] |
				DPLL_SEL_TRANSCODER_x_DPLL_SEL(Port, PLL));
	}
	Registers.Write(TRANS[Port].HTOTAL, Encode(Mode.H_Visible, Mode.H_Total));
	Registers.Write(TRANS[Port].HBLANK, Encode(Mode.H_Visible, Mode.H_Total));
	Registers.Write(TRANS[Port].HSYNC, Encode(Mode.H_Sync_Begin, Mode.H_Sync_End));
	Registers.Write(TRANS[Port].VTOTAL, Encode(Mode.V_Visible, Mode.V_Total));
	Registers.Write(TRANS[Port].VBLANK, Encode(Mode.V_Visible, Mode.V_Total));
	Registers.Write(TRANS[Port].VSYNC, Encode(Mode.V_Sync_Begin, Mode.V_Sync_End));
	
	if(Port_Cfg.Display == DP) {
		uint32_t Data_M, Link_M;
		uint32_t Data_N, Link_N;
		Calculate_M_N(Port_Cfg.DP, Port_Cfg.Mode, Data_M, Data_N, Link_M, Link_N);
		Registers.Write(TRANS[Port].DATA_M, TRANS_DATA_M_TU(64) | (uint32_t)(Data_M));
		Registers.Write(TRANS[Port].DATA_N, (uint32_t)(Data_N));
		Registers.Write(TRANS[Port].LINK_M, (uint32_t)(Link_M));
		Registers.Write(TRANS[Port].LINK_N, (uint32_t)(Link_N));
		if(CONFIG_Has_Trans_DP_Ctl) {
			// XXX Make conditional
			const uint32_t Polarity =
					TRANS_DP_CTL_HSYNC_ACTIVE_HIGH |
					TRANS_DP_CTL_VSYNC_ACTIVE_HIGH;
			const uint32_t Enhanced_Framing = TRANS_DP_CTL_ENHANCED_FRAMING;
			Registers.Write(TRANS[Port].DP_CTL,
					TRANS_DP_CTL_OUTPUT_ENABLE |
					TRANS_DP_CTL_PORT_SELECT[Port_Cfg.PCH_Port] |
					Enhanced_Framing |
					TRANS_DP_CTL_BPC(Port_Cfg.Mode.BPC) |
					Polarity
			);
		}
	}
	if(CONFIG_Has_Trans_Timing_Ovrrde) {
		Registers.Set_Mask(TRANS[Port].CHICKEN2, TRANS_CHICKEN2_TIMING_OVERRIDE);
	}
	Registers.Write(TRANS[Port].CONF, TRANS_CONF_TRANSCODER_ENABLE);
}

void Off(FDI_Port_Type Port)
{
	Registers.Unset_Mask(TRANS[Port].CONF, TRANS_CONF_TRANSCODER_ENABLE);
	Registers.Wait_Unset_Mask(TRANS[Port].CONF, TRANS_CONF_TRANSCODER_STATE, 50);
	if(CONFIG_Has_Trans_Timing_Ovrrde) {
		Registers.Unset_Mask(TRANS[Port].CHICKEN2,
				TRANS_CHICKEN2_TIMING_OVERRIDE);
	}
	if(CONFIG_Has_Trans_DP_Ctl) {
		Registers.Write(TRANS[Port].DP_CTL, TRANS_DP_CTL_PORT_SELECT_NONE);
	}
	if(CONFIG_Has_DPLL_SEL) {
		Registers.Unset_Mask(PCH_DPLL_SEL,
				DPLL_SEL_TRANSCODER_x_DPLL_ENABLE[Port]);
	}
}

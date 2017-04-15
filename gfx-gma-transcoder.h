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

#ifndef GFX_GMA_TRANSCODER_H
#define GFX_GMA_TRANSCODER_H

#include "gfx-gma-registers.h"

void Setup(Pipe_Index Pipe, Port_Config Port_Cfg);
void On(Pipe_Index Pipe, Port_Config Port_Cfg, Boolean Dither);
void Off(Pipe_Index Pipe);
void Clk_Off(Pipe_Index Pipe);
Word32 BPC_Conf(BPC_Type BPC, Boolean Dither);

typedef enum {
	Trans_EDP,
	Trans_A,
	Trans_B,
	Trans_C
} Transcoder_Index;

typedef struct t_Transcoder_Regs {
	Registers.Registers_Index HTOTAL;
	Registers.Registers_Index HBLANK;
	Registers.Registers_Index HSYNC;
	Registers.Registers_Index VTOTAL;
	Registers.Registers_Index VBLANK;
	Registers.Registers_Index VSYNC;
	Registers.Registers_Index CONF;
	Registers.Registers_Index DATA_M1;
	Registers.Registers_Index DATA_N1;
	Registers.Registers_Index LINK_M1;
	Registers.Registers_Index LINK_N1;
	Registers.Registers_Index DDI_FUNC_CTL;
	Registers.Registers_Index MSA_MISC;
	Registers.Registers_Invalid_Index CLK_SEL;
} Transcoder_Regs;

typedef Transcoder_Regs Transcoder_Array[4];
const Transcoder_Array Transcoders = {
	{
		Registers.HTOTAL_EDP,
		Registers.HBLANK_EDP,
		Registers.HSYNC_EDP,
		Registers.VTOTAL_EDP,
		Registers.VBLANK_EDP,
		Registers.VSYNC_EDP,
		Registers.PIPE_EDP_CONF,
		Registers.PIPE_EDP_DATA_M1,
		Registers.PIPE_EDP_DATA_N1,
		Registers.PIPE_EDP_LINK_M1,
		Registers.PIPE_EDP_LINK_N1,
		Registers.PIPE_EDP_DDI_FUNC_CTL,
		Registers.PIPE_EDP_MSA_MISC,
		Registers.Invalid_Register,
	},
	{
		Registers.HTOTAL_A,
		Registers.HBLANK_A,
		Registers.HSYNC_A,
		Registers.VTOTAL_A,
		Registers.VBLANK_A,
		Registers.VSYNC_A,
		Registers.PIPEACONF,
		Registers.PIPEA_DATA_M1,
		Registers.PIPEA_DATA_N1,
		Registers.PIPEA_LINK_M1,
		Registers.PIPEA_LINK_N1,
		Registers.PIPEA_DDI_FUNC_CTL,
		Registers.PIPEA_MSA_MISC,
		Registers.TRANSA_CLK_SEL
	},
	{
		Registers.HTOTAL_B,
		Registers.HBLANK_B,
		Registers.HSYNC_B,
		Registers.VTOTAL_B,
		Registers.VBLANK_B,
		Registers.VSYNC_B,
		Registers.PIPEBCONF,
		Registers.PIPEB_DATA_M1,
		Registers.PIPEB_DATA_N1,
		Registers.PIPEB_LINK_M1,
		Registers.PIPEB_LINK_N1,
		Registers.PIPEB_DDI_FUNC_CTL,
		Registers.PIPEB_MSA_MISC,
		Registers.TRANSB_CLK_SEL
	},
	{
		Registers.HTOTAL_C,
		Registers.HBLANK_C,
		Registers.HSYNC_C,
		Registers.VTOTAL_C,
		Registers.VBLANK_C,
		Registers.VSYNC_C,
		Registers.PIPECCONF,
		Registers.PIPEC_DATA_M1,
		Registers.PIPEC_DATA_N1,
		Registers.PIPEC_LINK_M1,
		Registers.PIPEC_LINK_N1,
		Registers.PIPEC_DDI_FUNC_CTL,
		Registers.PIPEC_MSA_MISC,
		Registers.TRANSC_CLK_SEL
	}
};

#endif

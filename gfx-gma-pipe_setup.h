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

#ifndef GFX_GMA_PIPE_SETUP_H
#define GFX_GMA_PIPE_SETUP_H

#include "gfx-gma-registers.h"

void On(Pipe_Index Pipe, Port_Config Port_Cfg, Framebuffer_Type Framebuffer);
void Off(Pipe_Index Pipe);
void Legacy_VGA_Off( void );
void All_Off( void );
void Update_Offset(Pipe_Index Pipe, Framebuffer_Type Framebuffer);
typedef Natural WM_Levels;
typedef Registers.Registers_Index PLANE_WM_Type[8];

typedef struct t_Controller_Type {
	Pipe_Index Pipe;
	Registers.Registers_Index PIPESRC;
	Registers.Registers_Index PIPEMISC;
	Registers.Registers_Index PF_CTRL;
	Registers.Registers_Index PF_WIN_POS;
	Registers.Registers_Index PF_WIN_SZ;
	Registers.Registers_Index DSPCNTR;
	Registers.Registers_Index DSPLINOFF;
	Registers.Registers_Index DSPSTRIDE;
	Registers.Registers_Index DSPSURF;
	Registers.Registers_Index DSPTILEOFF;
	Registers.Registers_Index SPCNTR;
	//  Skylake registers (partially aliased)
	Registers.Registers_Index PLANE_CTL;
	Registers.Registers_Index PLANE_OFFSET;
	Registers.Registers_Index PLANE_POS;
	Registers.Registers_Index PLANE_SIZE;
	Registers.Registers_Index PLANE_STRIDE;
	Registers.Registers_Index PLANE_SURF;
	Registers.Registers_Index PS_CTRL_1;
	Registers.Registers_Index PS_WIN_POS_1;
	Registers.Registers_Index PS_WIN_SZ_1;
	Registers.Registers_Invalid_Index PS_CTRL_2;
	Registers.Registers_Invalid_Index PS_WIN_SZ_2;
	Registers.Registers_Index WM_LINETIME;
	Registers.Registers_Index PLANE_BUF_CFG;
	PLANE_WM_Type PLANE_WM;
} Controller_Type;

typedef Controller_Type Controller_Array[3];

const Controller_Array Controllers = {
	{
		Primary,
		Registers.PIPEASRC,
		Registers.PIPEAMISC,
		Registers.PFA_CTL_1,
		Registers.PFA_WIN_POS,
		Registers.PFA_WIN_SZ,
		Registers.DSPACNTR,
		Registers.DSPALINOFF,
		Registers.DSPASTRIDE,
		Registers.DSPASURF,
		Registers.DSPATILEOFF,
		Registers.SPACNTR,
		Registers.DSPACNTR,
		Registers.DSPATILEOFF,
		Registers.PLANE_POS_1_A,
		Registers.PLANE_SIZE_1_A,
		Registers.DSPASTRIDE,
		Registers.DSPASURF,
		Registers.PS_CTRL_1_A,
		Registers.PS_WIN_POS_1_A,
		Registers.PS_WIN_SZ_1_A,
		Registers.PS_CTRL_2_A,
		Registers.PS_WIN_SZ_2_A,
		Registers.WM_LINETIME_A,
		Registers.PLANE_BUF_CFG_1_A,
		{
			Registers.PLANE_WM_1_A_0,
			Registers.PLANE_WM_1_A_1,
			Registers.PLANE_WM_1_A_2,
			Registers.PLANE_WM_1_A_3,
			Registers.PLANE_WM_1_A_4,
			Registers.PLANE_WM_1_A_5,
			Registers.PLANE_WM_1_A_6,
			Registers.PLANE_WM_1_A_7
		},
	},
	{
		Secondary,
		Registers.PIPEBSRC,
		Registers.PIPEBMISC,
		Registers.PFB_CTL_1,
		Registers.PFB_WIN_POS,
		Registers.PFB_WIN_SZ,
		Registers.DSPBCNTR,
		Registers.DSPBLINOFF,
		Registers.DSPBSTRIDE,
		Registers.DSPBSURF,
		Registers.DSPBTILEOFF,
		Registers.SPBCNTR,
		Registers.DSPBCNTR,
		Registers.DSPBTILEOFF,
		Registers.PLANE_POS_1_B,
		Registers.PLANE_SIZE_1_B,
		Registers.DSPBSTRIDE,
		Registers.DSPBSURF,
		Registers.PS_CTRL_1_B,
		Registers.PS_WIN_POS_1_B,
		Registers.PS_WIN_SZ_1_B,
		Registers.PS_CTRL_2_B,
		Registers.PS_WIN_SZ_2_B,
		Registers.WM_LINETIME_B,
		Registers.PLANE_BUF_CFG_1_B,
		{
			Registers.PLANE_WM_1_B_0,
			Registers.PLANE_WM_1_B_1,
			Registers.PLANE_WM_1_B_2,
			Registers.PLANE_WM_1_B_3,
			Registers.PLANE_WM_1_B_4,
			Registers.PLANE_WM_1_B_5,
			Registers.PLANE_WM_1_B_6,
			Registers.PLANE_WM_1_B_7
		},
	},
	{
		Tertiary,
		Registers.PIPECSRC,
		Registers.PIPECMISC,
		Registers.PFC_CTL_1,
		Registers.PFC_WIN_POS,
		Registers.PFC_WIN_SZ,
		Registers.DSPCCNTR,
		Registers.DSPCLINOFF,
		Registers.DSPCSTRIDE,
		Registers.DSPCSURF,
		Registers.DSPCTILEOFF,
		Registers.SPCCNTR,
		Registers.DSPCCNTR,
		Registers.DSPCTILEOFF,
		Registers.PLANE_POS_1_C,
		Registers.PLANE_SIZE_1_C,
		Registers.DSPCSTRIDE,
		Registers.DSPCSURF,
		Registers.PS_CTRL_1_C,
		Registers.PS_WIN_POS_1_C,
		Registers.PS_WIN_SZ_1_C,
		Registers.Invalid_Register,
		Registers.Invalid_Register,
		Registers.WM_LINETIME_C,
		Registers.PLANE_BUF_CFG_1_C,
		{
			Registers.PLANE_WM_1_C_0,
			Registers.PLANE_WM_1_C_1,
			Registers.PLANE_WM_1_C_2,
			Registers.PLANE_WM_1_C_3,
			Registers.PLANE_WM_1_C_4,
			Registers.PLANE_WM_1_C_5,
			Registers.PLANE_WM_1_C_6,
			Registers.PLANE_WM_1_C_7
		},
	},
};

#endif

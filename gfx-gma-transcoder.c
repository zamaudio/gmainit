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

#include "gfx-gma-transcoder.h"

typedef Transcoder_Index Default_Transcoder_Array[3];

const Default_Transcoder_Array Default_Transcoder = {
	Trans_A,
	Trans_B,
	Trans_C
};

Transcoder_Index Get_Idx(Pipe_Index Pipe, GPU_Port Port)
{
	if(Config.Has_EDP_Transcoder && (Port == DIGI_A)) {
		return Trans_EDP;
	} else {
		return Default_Transcoder[Pipe];
	}
	return Ret;
}

#define TRANS_CLK_SEL_PORT_NONE			(0 << 29)

typedef Word32 TRANS_CLK_SEL_PORT_Array[5];

const TRANS_CLK_SEL_PORT_Array TRANS_CLK_SEL_PORT = {
	TRANS_CLK_SEL_PORT_NONE, // DDI A is not selectable
	2 << 29,
	3 << 29,
	4 << 29,
	5 << 29
};

#define TRANS_CONF_ENABLE			(1 << 31)
#define TRANS_CONF_ENABLED_STATUS		(1 << 30)
#define TRANS_CONF_ENABLE_DITHER		(1 << 4)

typedef Word32 BPC_Array[5];

const BPC_Array TRANS_CONF_BPC = {
	/* 6  => */ 2 << 5,
	/* 8  => */ 0 << 5,
	/* 10 => */ 1 << 5,
	/* 12 => */ 3 << 5,
	0 << 5,
	0 << 5
};

Word32 BPC_Conf(BPC_Type BPC, Boolean Dither)
{
	Word32 Ret;
	if(Config.Has_Pipeconf_BPC) {
		Ret = TRANS_CONF_BPC[BPC];
	} else {
		Ret = 0;
	}
	if(Dither)
	{
		Ret |= TRANS_CONF_ENABLE_DITHER;
	}
	return Ret;
}

#define DDI_FUNC_CTL_ENABLE			(1 << 31)
#define DDI_FUNC_CTL_MODE_SELECT_MASK		(7 << 24)
#define DDI_FUNC_CTL_MODE_SELECT_HDMI		(0 << 24)
#define DDI_FUNC_CTL_MODE_SELECT_DVI		(1 << 24)
#define DDI_FUNC_CTL_MODE_SELECT_DP_SST		(2 << 24)
#define DDI_FUNC_CTL_MODE_SELECT_DP_MST		(3 << 24)
#define DDI_FUNC_CTL_MODE_SELECT_FDI		(4 << 24)
#define DDI_FUNC_CTL_EDP_SELECT_MASK		(7 << 12)
#define DDI_FUNC_CTL_EDP_SELECT_ALWAYS_ON	(0 << 12)
#define DDI_FUNC_CTL_EDP_SELECT_A		(4 << 12)
#define DDI_FUNC_CTL_EDP_SELECT_B		(5 << 12)
#define DDI_FUNC_CTL_EDP_SELECT_C		(6 << 12)

typedef Word32 DDI_Select_Array[5];

const DDI_Select_Array DDI_FUNC_CTL_DDI_SELECT = {
	/* DIGI_A => */ 0 << 28,
	/* DIGI_B => */ 1 << 28,
	/* DIGI_C => */ 2 << 28,
	/* DIGI_D => */ 3 << 28,
	/* DIGI_E => */ 4 << 28
};

typedef Word32 DDI_Mode_Array[5];

const DDI_Mode_Array DDI_FUNC_CTL_MODE_SELECT = {
	/* None => */ 0,
	/* LVDS => */ 0,
	/* DP => */ DDI_FUNC_CTL_MODE_SELECT_DP_SST,
	/* HDMI => */ DDI_FUNC_CTL_MODE_SELECT_DVI,
	/* VGA => */ DDI_FUNC_CTL_MODE_SELECT_FDI,
};

typedef Word32 HV_Sync_Array[2];

const HV_Sync_Array DDI_FUNC_CTL_VSYNC = {
	/* False => */ 0 << 17,
	/* True  => */ 1 << 17
};

const HV_Sync_Array DDI_FUNC_CTL_HSYNC = {
	/* False => */ 0 << 16,
	/* True  => */ 1 << 16
};

typedef Word32 EDP_Select_Array[3];

const EDP_Select_Array DDI_FUNC_CTL_EDP_SELECT = {
	/* Primary   => */ DDI_FUNC_CTL_EDP_SELECT_ALWAYS_ON,
	/* Secondary => */ DDI_FUNC_CTL_EDP_SELECT_B,
	/* Tertiary  => */ DDI_FUNC_CTL_EDP_SELECT_C
};

const EDP_Select_Array DDI_FUNC_CTL_EDP_SELECT_ONOFF = {
	/* Primary   => */ DDI_FUNC_CTL_EDP_SELECT_A,
	/* Secondary => */ DDI_FUNC_CTL_EDP_SELECT_B,
	/* Tertiary  => */ DDI_FUNC_CTL_EDP_SELECT_C
};

typedef Word32 Port_Width_Array[3];

const Port_Width_Array DDI_FUNC_CTL_PORT_WIDTH = {
	/* DP_Lane_Count_1 => */ 0 << 1,
	/* DP_Lane_Count_2 => */ 1 << 1,
	/* DP_Lane_Count_4 => */ 3 << 1
};

const BPC_Array DDI_FUNC_CTL_BPC = {
	/* 6  => */ 2 << 20,
	/* 8  => */ 0 << 20,
	/* 10 => */ 1 << 20,
	/* 12 => */ 3 << 20,
	0 << 20,
	0 << 20
};

#define TRANS_MSA_MISC_SYNC_CLK			(1 << 0)

const BPC_Array TRANS_MSA_MISC_BPC = {
	/* 6  => */ 0 << 5,
	/* 8  => */ 1 << 5,
	/* 10 => */ 2 << 5,
	/* 12 => */ 3 << 5,
	/* 16 => */ 4 << 5,
	1 << 5
};

Word32 TRANS_DATA_M_TU(Positive Transfer_Unit)
{
	return (Word32(Transfer_Unit - 1) << 25);
}

Word32 Encode(Pos16 LSW, Pos16 MSW)
{
	return (Word32(MSW - 1) << 16) | Word32(LSW - 1);
}

void Setup_Link(Transcoder_Regs Trans, DP_Link Link, Mode_Type Mode)
{
	DP_Info.M_Type Data_M, Link_M;
	DP_Info.N_Type Data_N, Link_N;

	DP_Info.Calculate_M_N(Link, Mode, Data_M, Data_N, Link_M, Link_N);
	Registers.Write(Trans.DATA_M1, TRANS_DATA_M_TU(64) | Word32(Data_M));
	Registers.Write(Trans.DATA_N1, Word32(Data_N));
	Registers.Write(Trans.LINK_M1, Word32(Link_M));
	Registers.Write(Trans.LINK_N1, Word32(Link_N));
	if(Config.Has_Pipe_MSA_Misc) {
		Registers.Write(Trans.MSA_MISC,
				TRANS_MSA_MISC_SYNC_CLK | TRANS_MSA_MISC_BPC[Mode.BPC]);
	}
}

void Setup(Pipe_Index Pipe, Port_Config Port_Cfg)
{
	const Mode_Type M = Port_Cfg.Mode;
	Transcoder_Regs Trans = Transcoders[Get_Idx(Pipe, Port_Cfg.Port)];

	if(Config.Has_Trans_Clk_Sel && (Trans.CLK_SEL != Registers.Invalid_Register)) {
		Registers.Write(Trans.CLK_SEL, TRANS_CLK_SEL_PORT[Port_Cfg.Port]);
	}
	if(Port_Cfg.Is_FDI) {
		Setup_Link(Trans, Port_Cfg.FDI, Port_Cfg.Mode);
	} else {
		if(Port_Cfg.Display == DP) {
			Setup_Link(Trans, Port_Cfg.DP, Port_Cfg.Mode);
		}
	}
	Registers.Write(Trans.HTOTAL, Encode(M.H_Visible, M.H_Total));
	Registers.Write(Trans.HBLANK, Encode(M.H_Visible, M.H_Total));
	Registers.Write(Trans.HSYNC, Encode(M.H_Sync_Begin, M.H_Sync_End));
	Registers.Write(Trans.VTOTAL, Encode(M.V_Visible, M.V_Total));
	Registers.Write(Trans.VBLANK, Encode(M.V_Visible, M.V_Total));
	Registers.Write(Trans.VSYNC, Encode(M.V_Sync_Begin, M.V_Sync_End));
}

void On(Pipe_Index Pipe, Port_Config Port_Cfg, Boolean Dither)
{
	Word32 Val;
	Transcoder_Regs Trans = Transcoders[Get_Idx(Pipe, Port_Cfg.Port)];
	if(Config.Has_Pipe_DDI_Func) {
		Registers.Write(Trans.DDI_FUNC_CTL,
				DDI_FUNC_CTL_ENABLE |
				DDI_FUNC_CTL_DDI_SELECT[Port_Cfg.Port] |
				DDI_FUNC_CTL_MODE_SELECT[Port_Cfg.Display] |
				DDI_FUNC_CTL_BPC[Port_Cfg.Mode.BPC] |
				DDI_FUNC_CTL_VSYNC[Port_Cfg.Mode.V_Sync_Active_High] |
				DDI_FUNC_CTL_HSYNC[Port_Cfg.Mode.H_Sync_Active_High] |
				DDI_FUNC_CTL_EDP_SELECT[Pipe] |
				DDI_FUNC_CTL_PORT_WIDTH[Port_Cfg.DP.Lane_Count]
		);
	}
	if(!Config.Has_Pipeconf_Misc) {
		Val = TRANS_CONF_ENABLE | BPC_Conf(Port_Cfg.Mode.BPC, Dither);
	} else {
		Val = TRANS_CONF_ENABLE;
	}
	Registers.Write(Trans.CONF, Val);
	Registers.Posting_Read(Trans.CONF);
}

void Trans_Off(Transcoder_Regs Trans)
{
	Boolean Enabled;
	Registers.Is_Set_Mask(Trans.CONF, TRANS_CONF_ENABLE, &Enabled);
	if(Enabled) {
		Registers.Unset_Mask(Trans.CONF, TRANS_CONF_ENABLE);
	}
	//  Workaround for Broadwell:
	//  Status may be wrong if pipe hasn't been enabled since reset.
	if(!Config.Pipe_Enabled_Workaround || Enabled) {
		//  synchronously wait until pipe is truly off
		Registers.Wait_Unset_Mask(Trans.CONF, TRANS_CONF_ENABLED_STATUS, 40);
	}
	if(Config.Has_Pipe_DDI_Func) {
		Registers.Write(Trans.DDI_FUNC_CTL, 0);
	}
}

void Off(Pipe_Index Pipe)
{
	Word32 DDI_Func_Ctl;
	if(Config.Has_EDP_Transcoder)
	{
		Registers.Read(Registers.PIPE_EDP_DDI_FUNC_CTL, &DDI_Func_Ctl);
		DDI_Func_Ctl &= DDI_FUNC_CTL_EDP_SELECT_MASK;
		if(((Pipe == Primary) &&
				(DDI_Func_Ctl == DDI_FUNC_CTL_EDP_SELECT_ALWAYS_ON)) ||
				(DDI_Func_Ctl == DDI_FUNC_CTL_EDP_SELECT_ONOFF[Pipe])) {
			Trans_Off(Transcoders[Trans_EDP]);
		}
	}
	Trans_Off(Transcoders[Default_Transcoder[Pipe]]);
}

void Clk_Off(Pipe_Index Pipe)
{
	Transcoder_Regs Trans = Transcoders[Default_Transcoder[Pipe]];

	if(Config.Has_Trans_Clk_Sel && (Trans.CLK_SEL != Registers.Invalid_Register)) {
		Registers.Write(Trans.CLK_SEL, TRANS_CLK_SEL_PORT_NONE);
	}
}

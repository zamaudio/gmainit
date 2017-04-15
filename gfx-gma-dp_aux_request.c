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

#include "gfx-gma-registers.h"
#include "gfx-gma-dp_aux_request.h"

#define DP_AUX_CTL_SEND_BUSY			(1 << 31)
#define DP_AUX_CTL_DONE				(1 << 30)
#define DP_AUX_CTL_INTERRUPT_ON_DONE		(1 << 29)
#define DP_AUX_CTL_TIME_OUT_ERROR		(1 << 28)
#define DP_AUX_CTL_TIME_OUT_TIMER_MASK		(3 << 26)
#define DP_AUX_CTL_TIME_OUT_TIMER_400US		(0 << 26)
#define DP_AUX_CTL_TIME_OUT_TIMER_600US		(1 << 26)
#define DP_AUX_CTL_TIME_OUT_TIMER_800US		(2 << 26)
#define DP_AUX_CTL_TIME_OUT_TIMER_1600US	(3 << 26)
#define DP_AUX_CTL_RECEIVE_ERROR		(1 << 25)
#define DP_AUX_CTL_MESSAGE_SIZE_MASK		(0x1f << 20)
#define DP_AUX_CTL_MESSAGE_SIZE_SHIFT		(1 << 20)
#define DP_AUX_CTL_PRECHARGE_TIME_MASK		(0xf << 16)
#define DP_AUX_CTL_PRECHARGE_TIME_SHIFT		(1 << 16)
#define DP_AUX_CTL_2X_BIT_CLOCK_DIV_MASK	0x7ff

// TODO: HSW/BDW with LPT-H might need a workaround for the 2x bit clock.
typedef Natural DP_AUX_CTL_MESSAGE_SIZE_T;

Word32 DP_AUX_CTL_MESSAGE_SIZE(DP_AUX_CTL_MESSAGE_SIZE_T Message_Length);

#define DDI_AUX_MUTEX_MUTEX_ENABLE		(1 << 31)
#define DDI_AUX_MUTEX_MUTEX_STATUS		(1 << 30)

typedef Positive AUX_CH_Data_Regs;
typedef Registers.Registers_Index AUX_CH_Data_Regs_Array[/* AUX_CH_Data_Regs */];
typedef struct t_AUX_CH_Registers
{
	Registers.Registers_Index CTL;
	AUX_CH_Data_Regs_Array DATA;
	Registers.Registers_Invalid_Index MUTEX;
} AUX_CH_Registers;

typedef AUX_CH_Registers AUX_CH_Registers_Array[4];

const AUX_CH_Registers_Array AUX_CH = {
	{
		Registers.DP_AUX_CTL_A,
		{
			Registers.DP_AUX_DATA_A_1,
			Registers.DP_AUX_DATA_A_2,
			Registers.DP_AUX_DATA_A_3,
			Registers.DP_AUX_DATA_A_4,
			Registers.DP_AUX_DATA_A_5
		},
		Registers.Invalid_Register
	},
	{
		Registers.DP_AUX_CTL_B,
		{
			Registers.DP_AUX_DATA_B_1,
			Registers.DP_AUX_DATA_B_2,
			Registers.DP_AUX_DATA_B_3,
			Registers.DP_AUX_DATA_B_4,
			Registers.DP_AUX_DATA_B_5
		},
		Registers.Invalid_Register
	},
	{
		Registers.DP_AUX_CTL_C,
		{
			Registers.DP_AUX_DATA_C_1,
			Registers.DP_AUX_DATA_C_2,
			Registers.DP_AUX_DATA_C_3,
			Registers.DP_AUX_DATA_C_4,
			Registers.DP_AUX_DATA_C_5
		},
		Registers.Invalid_Register
	},
	{
		Registers.DP_AUX_CTL_D,
		{
			Registers.DP_AUX_DATA_D_1,
			Registers.DP_AUX_DATA_D_2,
			Registers.DP_AUX_DATA_D_3,
			Registers.DP_AUX_DATA_D_4,
			Registers.DP_AUX_DATA_D_5
		},
		Registers.Invalid_Register
	}
};

Word32 DP_AUX_CTL_MESSAGE_SIZE(DP_AUX_CTL_MESSAGE_SIZE_T Message_Length)
{
	return Word32(Message_Length) * DP_AUX_CTL_MESSAGE_SIZE_SHIFT;
}

Boolean Aux_Request_Low(DP_Port Port, DP_Defs.Aux_Request Request, DP_Defs.Aux_Request_Length Request_Length, DP_Defs.Aux_Response *Response, DP_Defs.Aux_Response_Length *Response_Length)
{
	void Write_Data_Reg(Registers.Registers_Index Register, DP_Defs.Aux_Request Buf, DP_Defs.Aux_Request_Length Length, DP_Defs.Aux_Request_Index Offset)
	{
		Word32 Value;
		Natural Count;
		if(Offset < Length) {
			if(Length - Offset > 4) {
				Count = 4;
			} else {
				Count = Length - Offset;
			}
			Value = 0;
			for( int Idx = 0; Idx < Count; Idx++) {
				Value = Value | (Word32(Buf[Offset + Idx]) << ((3 - Idx) * 8));
			}
			Registers.Write(Register, Value);
		}
	}

	void Read_Data_Reg(Registers.Registers_Index Register, DP_Defs.Aux_Response *Buf, DP_Defs.Aux_Response_Length Length, DP_Defs.Aux_Response_Index Offset)
	{
		Word32 Value;
		DP_Defs.Aux_Response_Length Count;
		if(Offset < Length)
		{
			if(Length - Offset > 4) {
				Count = 4;
			} else {
				Count = Length - Offset;
			}
			Registers.Read(Register, Value);
			for( int Idx = 0; Idx < Count; Idx++)
			{
				Buf[Offset + Idx] = Word8((Value >> ((3 - Idx) * 8)) & 0xff);
			}
		}
	}

	Word32 DP_AUX_CTL_2x_Clock_Mask = DP_AUX_CTL_2X_BIT_CLOCK_DIV_MASK;
	const Word32 DP_AUX_CTL_2x_Clock = Word32((Config.Default_RawClk_Freq + 1000000) / 2000000);
	Boolean Busy;
	Word32 Status;
	//  Don't care
	*Response = 0;
	*Response_Length = DP_Defs.Aux_Response_Length[0];
	if(Config.Need_DP_Aux_Mutex) {
		Registers.Set_Mask(AUX_CH[Port].MUTEX, DDI_AUX_MUTEX_MUTEX_ENABLE);
		Registers.Wait_Set_Mask(AUX_CH[Port].MUTEX, DDI_AUX_MUTEX_MUTEX_STATUS);
	}
	Registers.Is_Set_Mask(AUX_CH[Port].CTL, DP_AUX_CTL_SEND_BUSY, &Busy);
	if(Busy) {
		Success = False;
	} else {
		for( int Idx = 0; Idx < 5; Idx++)
		{
			Write_Data_Reg(AUX_CH[Port].DATA(Idx), Request, Request_Length,
					(Natural(Idx) - 1) * 4);
		}

		Registers.Unset_And_Set_Mask(AUX_CH[Port].CTL,
			DP_AUX_CTL_INTERRUPT_ON_DONE |
			DP_AUX_CTL_TIME_OUT_TIMER_MASK |
			DP_AUX_CTL_MESSAGE_SIZE_MASK |
			DP_AUX_CTL_2x_Clock_Mask,
			DP_AUX_CTL_SEND_BUSY |
			DP_AUX_CTL_DONE |
			DP_AUX_CTL_TIME_OUT_ERROR |
			DP_AUX_CTL_RECEIVE_ERROR |
			DP_AUX_CTL_TIME_OUT_TIMER_600US |
			DP_AUX_CTL_MESSAGE_SIZE(Request_Length) |
			DP_AUX_CTL_2x_Clock);
		Registers.Wait_Unset_Mask(AUX_CH[Port].CTL, DP_AUX_CTL_SEND_BUSY);
		Registers.Read(AUX_CH[Port].CTL, &Status);
		Success = ((Status & (DP_AUX_CTL_TIME_OUT_ERROR | DP_AUX_CTL_RECEIVE_ERROR)) == 0);
		if(Success) {
			Status = (Status & DP_AUX_CTL_MESSAGE_SIZE_MASK) / DP_AUX_CTL_MESSAGE_SIZE_SHIFT;
			if(Natural(Status) < 0) {
				Success = False;
			} else {
				if(Natural(Status) > 31) {
					Response_Length = 31;
				} else {
					Response_Length = Natural(Status);
				}
			}
		}
		if(Success) {
			for( int Idx = 0; Idx < 5; Idx++)
			{
				Read_Data_Reg(AUX_CH[Port].DATA(Idx),
						Response, Response_Length,
						(Natural(Idx) - 1) * 4);
			}
		}
	}
	if(Config.Need_DP_Aux_Mutex) {
		Registers.Unset_And_Set_Mask(AUX_CH[Port].MUTEX,
				DDI_AUX_MUTEX_MUTEX_ENABLE,
				DDI_AUX_MUTEX_MUTEX_STATUS);
	}
}

// --------------------------------------------------------------------------
Boolean Do_Aux_Request(DP_Port Port, DP_Defs.Aux_Request Request, DP_Defs.Aux_Request_Length Request_Length, DP_Defs.Aux_Response *Response, DP_Defs.Aux_Response_Length *Response_Length)
{
	Boolean Success = False;
	for( int Try = 1; Try <= 3; Try++) {
		Success = Aux_Request_Low(Port, Request, Request_Length, Response, Response_Length);
		if (Success) {
			break;
		}
	}
	return Success;
}

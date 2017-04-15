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
typedef uint32_t DP_AUX_CTL_MESSAGE_SIZE_T;

uint32_t DP_AUX_CTL_MESSAGE_SIZE(DP_AUX_CTL_MESSAGE_SIZE_T Message_Length);

#define DDI_AUX_MUTEX_MUTEX_ENABLE		(1 << 31)
#define DDI_AUX_MUTEX_MUTEX_STATUS		(1 << 30)

typedef uint32_t AUX_CH_Data_Regs_Array[5];
typedef struct t_AUX_CH_Registers {
	uint32_t CTL;
	AUX_CH_Data_Regs_Array DATA;
	uint32_t MUTEX;
} AUX_CH_Registers;

typedef AUX_CH_Registers AUX_CH_Registers_Array[4];

const AUX_CH_Registers_Array AUX_CH = {
	{
		DP_AUX_CTL_A,
		{
			DP_AUX_DATA_A_1,
			DP_AUX_DATA_A_2,
			DP_AUX_DATA_A_3,
			DP_AUX_DATA_A_4,
			DP_AUX_DATA_A_5
		},
		INVALID_REGISTER
	},
	{
		PCH_DP_AUX_CTL_B,
		{
			PCH_DP_AUX_DATA_B_1,
			PCH_DP_AUX_DATA_B_2,
			PCH_DP_AUX_DATA_B_3,
			PCH_DP_AUX_DATA_B_4,
			PCH_DP_AUX_DATA_B_5
		},
		INVALID_REGISTER
	},
	{
		PCH_DP_AUX_CTL_C,
		{
			PCH_DP_AUX_DATA_C_1,
			PCH_DP_AUX_DATA_C_2,
			PCH_DP_AUX_DATA_C_3,
			PCH_DP_AUX_DATA_C_4,
			PCH_DP_AUX_DATA_C_5
		},
		INVALID_REGISTER
	},
	{
		PCH_DP_AUX_CTL_D,
		{
			PCH_DP_AUX_DATA_D_1,
			PCH_DP_AUX_DATA_D_2,
			PCH_DP_AUX_DATA_D_3,
			PCH_DP_AUX_DATA_D_4,
			PCH_DP_AUX_DATA_D_5
		},
		INVALID_REGISTER
	}
};

uint32_t DP_AUX_CTL_MESSAGE_SIZE(DP_AUX_CTL_MESSAGE_SIZE_T Message_Length)
{
	return (uint32_t)(Message_Length) * DP_AUX_CTL_MESSAGE_SIZE_SHIFT;
}

bool Aux_Request_Low(DP_Port Port, Aux_Request Request, Aux_Request_Length Request_Length, Aux_Response *Response, Aux_Response_Length *Response_Length)
{
	void Write_Data_Reg(uint32_t Register, Aux_Request Buf, Aux_Request_Length Length, Aux_Request_Index Offset)
	{
		uint32_t Value;
		uint32_t Count;
		if(Offset < Length) {
			if(Length - Offset > 4) {
				Count = 4;
			} else {
				Count = Length - Offset;
			}
			Value = 0;
			for( int Idx = 0; Idx < Count; Idx++) {
				Value = Value | ((uint32_t)(Buf[Offset + Idx]) << ((3 - Idx) * 8));
			}
			Registers.Write(Register, Value);
		}
	}

	void Read_Data_Reg(uint32_t Register, Aux_Response *Buf, Aux_Response_Length Length, Aux_Response_Index Offset)
	{
		uint32_t Value;
		Aux_Response_Length Count;
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
				(*Buf)[Offset + Idx] = (uint8_t)((Value >> ((3 - Idx) * 8)) & 0xff);
			}
		}
	}

	uint32_t DP_AUX_CTL_2x_Clock_Mask = DP_AUX_CTL_2X_BIT_CLOCK_DIV_MASK;
	const uint32_t DP_AUX_CTL_2x_Clock = (uint32_t)((CONFIG_Default_RawClk_Freq + 1000000) / 2000000);
	bool Busy;
	uint32_t Status;
	bool Success = false;
	//  Don't care
	(*Response)[0] = 0;
	*Response_Length = 0;
	if(CONFIG_Need_DP_Aux_Mutex) {
		Registers.Set_Mask(AUX_CH[Port].MUTEX, DDI_AUX_MUTEX_MUTEX_ENABLE);
		Registers.Wait_Set_Mask(AUX_CH[Port].MUTEX, DDI_AUX_MUTEX_MUTEX_STATUS);
	}
	Registers.Is_Set_Mask(AUX_CH[Port].CTL, DP_AUX_CTL_SEND_BUSY, &Busy);
	if(Busy) {
		Success = false;
	} else {
		for( int Idx = 0; Idx < 5; Idx++)
		{
			Write_Data_Reg(AUX_CH[Port].DATA[Idx], Request, Request_Length,
					((uint32_t)(Idx) - 1) * 4);
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
			if((uint32_t)(Status) < 0) {
				Success = false;
			} else {
				if((uint32_t)(Status) > 31) {
					*Response_Length = 31;
				} else {
					*Response_Length = (uint32_t)(Status);
				}
			}
		}
		if(Success) {
			for( int Idx = 0; Idx < 5; Idx++)
			{
				Read_Data_Reg(AUX_CH[Port].DATA[Idx],
						Response, *Response_Length,
						((uint32_t)(Idx) - 1) * 4);
			}
		}
	}
	if(CONFIG_Need_DP_Aux_Mutex) {
		Registers.Unset_And_Set_Mask(AUX_CH[Port].MUTEX,
				DDI_AUX_MUTEX_MUTEX_ENABLE,
				DDI_AUX_MUTEX_MUTEX_STATUS);
	}
}

// --------------------------------------------------------------------------
bool Do_Aux_Request(DP_Port Port, Aux_Request Request, Aux_Request_Length Request_Length, Aux_Response *Response, Aux_Response_Length *Response_Length)
{
	bool Success = false;
	for( int Try = 1; Try <= 3; Try++) {
		Success = Aux_Request_Low(Port, Request, Request_Length, Response, Response_Length);
		if (Success) {
			break;
		}
	}
	return Success;
}

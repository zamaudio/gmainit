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
#include "gfx-gma-pch-fdi.h"

#define FDI_RX_CTL_FDI_RX_ENABLE		(1 << 31)
#define FDI_RX_CTL_FS_ERROR_CORRECTION_ENABLE	(1 << 27)
#define FDI_RX_CTL_FE_ERROR_CORRECTION_ENABLE	(1 << 26)
#define FDI_RX_CTL_PORT_WIDTH_SEL_SHIFT		19
#define FDI_RX_CTL_FDI_PLL_ENABLE		(1 << 13)
#define FDI_RX_CTL_COMPOSITE_SYNC_SELECT	(1 << 11)
#define FDI_RX_CTL_FDI_AUTO_TRAIN		(1 << 10)
#define FDI_RX_CTL_ENHANCED_FRAMING_ENABLE	(1 << 6)
#define FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_MASK	(1 << 4)
#define FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_RAWCLK	(0 << 4)
#define FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_PCDCLK	(1 << 4)

// XXX TP_SHIFT 	(if Config.CPU = Ironlake then 28 else 8);
#define TP_SHIFT				8
#define FDI_RX_CTL_TRAINING_PATTERN_MASK	(3 << TP_SHIFT)

typedef uint32_t TP_Array[4];

const TP_Array FDI_RX_CTL_TRAINING_PATTERN = {
	0 << TP_SHIFT,
	1 << TP_SHIFT,
	2 << TP_SHIFT,
	3 << TP_SHIFT
};

uint32_t FDI_RX_CTL_PORT_WIDTH_SEL(DP_Lane_Count Lane_Count)
{
	return (((uint32_t)(Lane_Count_As_Integer[Lane_Count]) - 1) << FDI_RX_CTL_PORT_WIDTH_SEL_SHIFT);
}

uint32_t FDI_RX_CTL_BPC(BPC_Type BPC)
{
	switch(BPC) {
	case 6:
		return 2 << 16;
	case 10:
		return 1 << 16;
	case 12:
		return 3 << 16;
	default:
		return 0;
	}
}

#define FDI_RX_MISC_FDI_RX_PWRDN_LANE1_SHIFT	26
#define FDI_RX_MISC_FDI_RX_PWRDN_LANE1_MASK	(3 << 26)
#define FDI_RX_MISC_FDI_RX_PWRDN_LANE0_SHIFT	24
#define FDI_RX_MISC_FDI_RX_PWRDN_LANE0_MASK	(3 << 24)
#define FDI_RX_MISC_TP1_TO_TP2_TIME_48		(2 << 20)
#define FDI_RX_MISC_FDI_DELAY_90		90

uint32_t FDI_RX_MISC_FDI_RX_PWRDN_LANE1(uint32_t Value)
{
	return (Value << FDI_RX_MISC_FDI_RX_PWRDN_LANE1_SHIFT);
}

uint32_t FDI_RX_MISC_FDI_RX_PWRDN_LANE0(uint32_t Value)
{
	return (Value << FDI_RX_MISC_FDI_RX_PWRDN_LANE0_SHIFT);
}

#define FDI_RX_TUSIZE_SHIFT			25

uint32_t FDI_RX_TUSIZE(uint32_t Value)
{
	return ((Value - 1) << FDI_RX_TUSIZE_SHIFT);
}

#define FDI_RX_INTERLANE_ALIGNMENT		(1 << 10)
#define FDI_RX_SYMBOL_LOCK			(1 << 9)
#define FDI_RX_BIT_LOCK				(1 << 8)

typedef struct t_FDI_Registers
{
	uint32_t RX_CTL;
	uint32_t RX_MISC;
	uint32_t RX_TUSIZE;
	uint32_t RX_IMR;
	uint32_t RX_IIR;
} FDI_Registers;

typedef FDI_Registers FDI_Registers_Array[3];

const FDI_Registers_Array FDI_Regs = {
	{
		FDI_RXA_CTL,
		FDI_RX_MISC_A,
		FDI_RXA_TUSIZE1,
		FDI_RXA_IMR,
		FDI_RXA_IIR
	},
	{
		FDI_RXB_CTL,
		FDI_RX_MISC_B,
		FDI_RXB_TUSIZE1,
		FDI_RXB_IMR, 
		FDI_RXB_IIR
	},
	{
		FDI_RXC_CTL,
		FDI_RX_MISC_C,
		FDI_RXC_TUSIZE1,
		FDI_RXC_IMR,
		FDI_RXC_IIR
	}
};

void Pre_Train(FDI_Port_Type Port, Port_Config Port_Cfg)
{
	const uint32_t Power_Down_Lane_Bits = FDI_RX_MISC_FDI_RX_PWRDN_LANE1(2) |
			FDI_RX_MISC_FDI_RX_PWRDN_LANE0(2);
	const uint32_t RX_CTL_Settings =
			FDI_RX_CTL_PORT_WIDTH_SEL(Port_Cfg.FDI.Lane_Count) |
			FDI_RX_CTL_BPC(Port_Cfg.Mode.BPC) |
			FDI_RX_CTL_COMPOSITE_SYNC_SELECT |
			FDI_RX_CTL_ENHANCED_FRAMING_ENABLE;

	//  TODO: HSW: check DISPIO_CR_TX_BMU_CR4, seems Linux doesn't know it
	Registers.Write(FDI_Regs[Port].RX_MISC,
			Power_Down_Lane_Bits |
			FDI_RX_MISC_TP1_TO_TP2_TIME_48 |
			FDI_RX_MISC_FDI_DELAY_90
	);
	Registers.Write(FDI_Regs[Port].RX_TUSIZE, FDI_RX_TUSIZE(64));
	Registers.Unset_Mask(FDI_Regs[Port].RX_IMR,
			FDI_RX_INTERLANE_ALIGNMENT |
			FDI_RX_SYMBOL_LOCK |
			FDI_RX_BIT_LOCK);
	Registers.Posting_Read(FDI_Regs[Port].RX_IMR);

	//  clear stale lock bits
	Registers.Write(FDI_Regs[Port].RX_IIR,
			FDI_RX_INTERLANE_ALIGNMENT |
			FDI_RX_SYMBOL_LOCK |
			FDI_RX_BIT_LOCK);

	Registers.Write(FDI_Regs[Port].RX_CTL,
			FDI_RX_CTL_FDI_PLL_ENABLE | RX_CTL_Settings);
	Registers.Posting_Read(FDI_Regs[Port].RX_CTL);
	Time.U_Delay(220);
	Registers.Set_Mask(FDI_Regs[Port].RX_CTL, FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_PCDCLK);
}

bool Train(FDI_Port_Type Port, Training_Pattern TP)
{
	bool Success = false;
	const uint32_t Lock_Bit = (TP == TP_1) ? FDI_RX_BIT_LOCK : FDI_RX_SYMBOL_LOCK;
	
	bool Check_Lock(uint32_t Lock_Bit)
	{
		for( int I = 0; I < 5; I++)
		{
			Registers.Is_Set_Mask(FDI_Regs[Port].RX_IIR, Lock_Bit, &Success);
			if(Success) {
				//  clear the lock bit
				Registers.Write(FDI_Regs[Port].RX_IIR, Lock_Bit);
			}
			break;
			udelay(1);
		}
		return Success;
	}

	Registers.Unset_And_Set_Mask(FDI_Regs[Port].RX_CTL,
			FDI_RX_CTL_TRAINING_PATTERN_MASK,
			FDI_RX_CTL_FDI_RX_ENABLE |
			FDI_RX_CTL_TRAINING_PATTERN[TP]
	);
	Registers.Posting_Read(FDI_Regs[Port].RX_CTL);
	if(TP <= TP_2) {
		udelay(1);
		if(TP == TP_1) {
			Success = Check_Lock(FDI_RX_BIT_LOCK);
		} else {
			Success = Check_Lock(FDI_RX_SYMBOL_LOCK);
			if(Success) {
				Success = Check_Lock(FDI_RX_INTERLANE_ALIGNMENT);
			}
		}
	} else {
		udelay(31);
		Success = true;
	}
	return Success;
}

void Auto_Train(FDI_Port_Type Port)
{
	Registers.Set_Mask(FDI_Regs[Port].RX_CTL,
			FDI_RX_CTL_FDI_RX_ENABLE |
			FDI_RX_CTL_FDI_AUTO_TRAIN
	);
	Registers.Posting_Read(FDI_Regs[Port].RX_CTL);
	if(CONFIG_Has_FDI_RX_Power_Down) {
		udelay(30);
		Registers.Unset_And_Set_Mask(FDI_Regs[Port].RX_MISC,
				FDI_RX_MISC_FDI_RX_PWRDN_LANE1_MASK |
				FDI_RX_MISC_FDI_RX_PWRDN_LANE0_MASK,
				FDI_RX_MISC_FDI_RX_PWRDN_LANE1(0) |
				FDI_RX_MISC_FDI_RX_PWRDN_LANE0(0)
		);
		Registers.Posting_Read(FDI_Regs[Port].RX_MISC);
	}
	udelay(5);
}

void Enable_EC(FDI_Port_Type Port)
{
	Registers.Set_Mask(FDI_Regs[Port].RX_CTL,
			FDI_RX_CTL_FS_ERROR_CORRECTION_ENABLE |
			FDI_RX_CTL_FE_ERROR_CORRECTION_ENABLE);
}

void Off(FDI_Port_Type Port, Off_Type OT)
{
	Registers.Unset_Mask(FDI_Regs[Port].RX_CTL,
			FDI_RX_CTL_FDI_RX_ENABLE | FDI_RX_CTL_FDI_AUTO_TRAIN);
	if(CONFIG_Has_FDI_RX_Power_Down && (OT >= Lanes_Off)) {
		Registers.Unset_And_Set_Mask(FDI_Regs[Port].RX_MISC,
				FDI_RX_MISC_FDI_RX_PWRDN_LANE1_MASK |
				FDI_RX_MISC_FDI_RX_PWRDN_LANE0_MASK,
				FDI_RX_MISC_FDI_RX_PWRDN_LANE1(2) |
				FDI_RX_MISC_FDI_RX_PWRDN_LANE0(2)
		);
		Registers.Posting_Read(FDI_Regs[Port].RX_MISC);
	}
	if(OT >= Clock_Off) {
		Registers.Unset_And_Set_Mask(FDI_Regs[Port].RX_CTL,
				FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_MASK,
				FDI_RX_CTL_RAWCLK_TO_PCDCLK_SEL_RAWCLK);
		Registers.Unset_Mask(FDI_Regs[Port].RX_CTL, FDI_RX_CTL_FDI_PLL_ENABLE);
	}
}

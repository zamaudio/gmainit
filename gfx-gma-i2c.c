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
#include "gfx-gma-i2c.h"

#define PCH_DSPCLK_GATE_D_GMBUS_UNIT_LVL	(1 << 31)
#define GMBUS0_GMBUS_RATE_SELECT_MASK		(7 << 8)
#define GMBUS0_GMBUS_RATE_SELECT_100KHZ		(0 << 8)
#define GMBUS0_GMBUS_RATE_SELECT_50KHZ		(1 << 8)
#define GMBUS0_PIN_PAIR_SELECT_MASK		(7 << 0)
#define GMBUS0_PIN_PAIR_SELECT_NONE		(0 << 0)
#define GMBUS0_PIN_PAIR_SELECT_DAC		(2 << 0)
#define GMBUS0_PIN_PAIR_SELECT_LVDS		(3 << 0)
// Order is C, B, D: no typo!
#define GMBUS0_PIN_PAIR_SELECT_DIGI_C		(4 << 0)
#define GMBUS0_PIN_PAIR_SELECT_DIGI_B		(5 << 0)
#define GMBUS0_PIN_PAIR_SELECT_DIGI_D		(6 << 0)
#define GMBUS1_SOFTWARE_CLEAR_INTERRUPT		(1 << 31)
#define GMBUS1_SOFTWARE_READY			(1 << 30)
#define GMBUS1_ENABLE_TIMEOUT			(1 << 29)
#define GMBUS1_BUS_CYCLE_SELECT_MASK		(7 << 25)
#define GMBUS1_BUS_CYCLE_STOP			(1 << 27)
#define GMBUS1_BUS_CYCLE_INDEX			(1 << 26)
#define GMBUS1_BUS_CYCLE_WAIT			(1 << 25)
#define GMBUS1_TOTAL_BYTE_COUNT_MASK		(0x1ff << 16)
#define GMBUS1_TOTAL_BYTE_COUNT_SHIFT		16
#define GMBUS1_8BIT_SLAVE_INDEX_MASK		(0xff << 8)
#define GMBUS1_8BIT_SLAVE_INDEX_SHIFT		8
#define GMBUS1_SLAVE_ADDRESS_MASK		(0x7f << 1)
#define GMBUS1_SLAVE_ADDRESS_SHIFT		1
#define GMBUS1_DIRECTION_MASK			(1 << 0)
#define GMBUS1_DIRECTION_WRITE			(0 << 0)
#define GMBUS1_DIRECTION_READ			(1 << 0)
#define GMBUS2_INUSE				(1 << 15)
#define GMBUS2_HARDWARE_WAIT_PHASE		(1 << 14)
#define GMBUS2_SLAVE_STALL_TIMEOUT_ERROR	(1 << 13)
#define GMBUS2_GMBUS_INTERRUPT_STATUS		(1 << 12)
#define GMBUS2_HARDWARE_READY			(1 << 11)
#define GMBUS2_NAK_INDICATOR			(1 << 10)
#define GMBUS2_GMBUS_ACTIVE			(1 << 9)
#define GMBUS2_CURRENT_BYTE_COUNT_MASK		(0x1ff << 0)
#define GMBUS4_INTERRUPT_MASK			(0x1f << 0)
#define GMBUS5_2BYTE_INDEX_ENABLE		(1 << 31)

Word32 GMBUS1_TOTAL_BYTE_COUNT(Transfer_Length Count)
{
	return (Word32(Count) << GMBUS1_TOTAL_BYTE_COUNT_SHIFT);
}

Word32 GMBUS1_SLAVE_ADDRESS(Transfer_Address Address)
{
	return (Word32(Address) << GMBUS1_SLAVE_ADDRESS_SHIFT);
}

Word32 GMBUS0_PIN_PAIR_SELECT(PCH_Port Port)
{
	switch(Port) {
	case PCH_DAC:
		return GMBUS0_PIN_PAIR_SELECT_DAC;
	case PCH_LVDS:
		return GMBUS0_PIN_PAIR_SELECT_LVDS;
	case PCH_HDMI_B:
		return GMBUS0_PIN_PAIR_SELECT_DIGI_B;
	case PCH_HDMI_C:
		return GMBUS0_PIN_PAIR_SELECT_DIGI_C;
	case PCH_HDMI_D:
		return GMBUS0_PIN_PAIR_SELECT_DIGI_D;
	default:
		return GMBUS0_PIN_PAIR_SELECT_NONE;
	}
}

// --------------------------------------------------------------------------
Boolean GMBUS_Ready(void)
{
	Word32 GMBUS2;
	Registers.Read(Registers.PCH_GMBUS2, &GMBUS2);
	return (GMBUS2 & (
			GMBUS2_HARDWARE_WAIT_PHASE |
			GMBUS2_SLAVE_STALL_TIMEOUT_ERROR |
			GMBUS2_GMBUS_INTERRUPT_STATUS |
			GMBUS2_NAK_INDICATOR
			)
		) == 0;
}

Boolean Reset_GMBUS(void)
{
	Registers.Write(Registers.PCH_GMBUS1, GMBUS1_SOFTWARE_CLEAR_INTERRUPT);
	Registers.Write(Registers.PCH_GMBUS1, 0);
	Registers.Write(Registers.PCH_GMBUS0, GMBUS0_PIN_PAIR_SELECT_NONE);
	return GMBUS_Ready();
}

Boolean Init_GMBUS(PCH_Port Port)
{
	Boolean Success;

	if(Config.Ungate_GMBUS_Unit_Level) {
	  Registers.Set_Mask(Registers.PCH_DSPCLK_GATE_D, PCH_DSPCLK_GATE_D_GMBUS_UNIT_LVL);
	}
	//  TODO: Refactor + check for timeout.
	Registers.Wait_Unset_Mask(Registers.PCH_GMBUS2, GMBUS2_INUSE);
	Success = GMBUS_Ready();
	if(!Success) {
		Success = Reset_GMBUS();
	}
	if(Success) {
	  Registers.Write(Registers.PCH_GMBUS0, GMBUS0_GMBUS_RATE_SELECT_100KHZ |
	  					GMBUS0_PIN_PAIR_SELECT(Port));
	  Registers.Write(Registers.PCH_GMBUS4, 0);
	  Registers.Write(Registers.PCH_GMBUS5, 0);
	}
	return Success;
}

void Release_GMBUS( void )
{
	Registers.Write(Registers.PCH_GMBUS0, GMBUS0_PIN_PAIR_SELECT_NONE);
	//  Clear INUSE. TODO: Don't do it, if timeout occured (see above).
	Registers.Write(Registers.PCH_GMBUS2, GMBUS2_INUSE);
	if(Config.Ungate_GMBUS_Unit_Level) {
		Registers.Unset_Mask(Registers.PCH_DSPCLK_GATE_D,
				PCH_DSPCLK_GATE_D_GMBUS_UNIT_LVL);
	}
}

Boolean I2C_Read(PCH_Port Port, Transfer_Address Address, Transfer_Length *Length, Transfer_Data *Data)
{
	Boolean Success;
	Word32 GMBUS2, GMBUS3;
	Transfer_Length Current;
	Transfer_Length Transfered = 0;
	//Data = (/* others => */ 0);
	Success = Init_GMBUS(Port);
	if(Success) {
		Registers.Write(Registers.PCH_GMBUS1,
				GMBUS1_SOFTWARE_READY |
				GMBUS1_BUS_CYCLE_INDEX |
				GMBUS1_BUS_CYCLE_WAIT |
				GMBUS1_TOTAL_BYTE_COUNT(*Length) |
				GMBUS1_SLAVE_ADDRESS(Address) |
				GMBUS1_DIRECTION_READ);
		while( Success && Transfered < Length)
		{
			Registers.Wait_Set_Mask(Registers.PCH_GMBUS2, GMBUS2_HARDWARE_READY, 55);
			Registers.Read(Registers.PCH_GMBUS2, &GMBUS2);
			Success = ((GMBUS2 & GMBUS2_HARDWARE_READY) != 0) &&
					((GMBUS2 & GMBUS2_NAK_INDICATOR) == 0);
			if(Success) {
				Current = min(Length, Transfered + 4);
				Registers.Read(Registers.PCH_GMBUS3, GMBUS3);
				for( int I = Transfered; I < Current; I++) {
					Data[I] = Byte(GMBUS3 & 0xff);
					GMBUS3 = GMBUS3 >> 8;
				}
				Transfered = Current;
			}
		}

		if(Success) {
			Registers.Wait_Set_Mask(Registers.PCH_GMBUS2, GMBUS2_HARDWARE_WAIT_PHASE);
			Registers.Write(Registers.PCH_GMBUS1, GMBUS1_SOFTWARE_READY |
								GMBUS1_BUS_CYCLE_STOP);
			Registers.Wait_Unset_Mask(Registers.PCH_GMBUS2, GMBUS2_GMBUS_ACTIVE);
		}
	}
	Length = Transfered;
	Release_GMBUS();
	return Success;
}

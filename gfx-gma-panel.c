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

#include "gfx-gma-panel.h"

typedef enum {
	Power_Up_Delay,
	Power_Up_To_BL_On,
	Power_Down_Delay,
	BL_Off_To_Power_Down,
	Power_Cycle_Delay
} Delays_Enum;

typedef Natural Panel_Power_Delays[5];

const Panel_Power_Delays Default_EDP_Delays_US = {
	210000,
	50000,
	500000,
	50000,
	510000
};

Panel_Power_Delays Delays_US;

// --------------------------------------------------------------------------
//  And here the mess starts: We have this pretty hardware power sequencer
//  that should ensure the panel's timing constraints are satisfied. But
//  (at least on some generations) it doesn't do it's job. On Haswell, it
//  seems to ignore the Power_Cycle_Delay, so we ensure the delay in soft-
//  ware. On at least Ivy Bridge and Broadwell Power_Up_Delay is ignored.
// 
//  If we ever do all delays in software, there are two ways: Either confi-
//  gure the hardware to zero delays or wait for both the software timeout
//  and the hardware power sequencer. The latter option would be less error
//  prone, as the hardware might just don't work as expected.
Time.T Power_Cycle_Timer;
Time.T Power_Up_Timer;

// --------------------------------------------------------------------------
Word32 Div_Round_Up32(Natural Numerator, Natural Denominator)
{
	return (Word32(Numerator) + Word32(Denominator) - 1) / Word32(Denominator);
}

#define PCH_PP_STATUS_ENABLED			(1 << 31)
#define PCH_PP_STATUS_REQUIRE_ASSET		(1 << 30)
#define PCH_PP_STATUS_PWR_SEQ_PROGRESS_MASK	(3 << 28)
#define PCH_PP_STATUS_PWR_SEQ_PROGRESS_NONE	(0 << 28)
#define PCH_PP_STATUS_PWR_SEQ_PROGRESS_UP	(1 << 28)
#define PCH_PP_STATUS_PWR_SEQ_PROGRESS_DOWN	(2 << 28)
#define PCH_PP_STATUS_PWR_CYC_DELAY_ACTIVE	(1 << 27)
#define PCH_PP_CONTROL_WRITE_PROTECT_MASK	(0xffff << 16)
#define PCH_PP_CONTROL_WRITE_PROTECT_KEY	(0xabcd << 16)
#define PCH_PP_CONTROL_VDD_OVERRIDE		(1 << 3)
#define PCH_PP_CONTROL_BACKLIGHT_ENABLE		(1 << 2)
#define PCH_PP_CONTROL_POWER_DOWN_ON_RESET	(1 << 1)
#define PCH_PP_CONTROL_TARGET_ON		(1 << 0)
#define PCH_PP_ON_DELAYS_PORT_SELECT_MASK	(3 << 30)
#define PCH_PP_ON_DELAYS_PORT_SELECT_LVDS	(0 << 30)
#define PCH_PP_ON_DELAYS_PORT_SELECT_DP_A	(1 << 30)
#define PCH_PP_ON_DELAYS_PORT_SELECT_DP_C	(2 << 30)
#define PCH_PP_ON_DELAYS_PORT_SELECT_DP_D	(3 << 30)
#define PCH_PP_ON_DELAYS_PWR_UP_MASK		(0x1fff << 16)
#define PCH_PP_ON_DELAYS_PWR_UP_BL_ON_MASK	(0x1fff << 0)

Word32 PCH_PP_ON_DELAYS_PWR_UP(Natural US)
{
	return (Div_Round_Up32(US, 100) << 16);
}

Word32 PCH_PP_ON_DELAYS_PWR_UP_BL_ON(Natural US)
{
	return Div_Round_Up32(US, 100);
}

#define PCH_PP_OFF_DELAYS_PWR_DOWN_MASK		(0x1fff << 16)
#define PCH_PP_OFF_DELAYS_BL_OFF_PWR_DOWN_MASK	(0x1fff << 0)

Word32 PCH_PP_OFF_DELAYS_PWR_DOWN(Natural US)
{
	return (Div_Round_Up32(US, 100) << 16);
}

Word32 PCH_PP_OFF_DELAYS_BL_OFF_PWR_DOWN(Natural US)
{
	return Div_Round_Up32(US, 100);
}

  PCH_PP_DIVISOR_REF_DIVIDER_MASK	ffffff  << 8;
  PCH_PP_DIVISOR_PWR_CYC_DELAY_MASK	00001f  << 0;

Word32 PCH_PP_DIVISOR_PWR_CYC_DELAY(Natural US)
{
	return Div_Round_Up32(US, 100000) + 1;
}

#define CPU_BLC_PWM_CTL_ENABLE			(1 << 31)
#define CPU_BLC_PWM_CTL_PIPE_SELECT_MASK	(3 << 29)
#define CPU_BLC_PWM_CTL_PIPE_SELECT_PIPE_A	(0 << 29)
#define CPU_BLC_PWM_CTL_PIPE_SELECT_PIPE_B	(1 << 29)
#define CPU_BLC_PWM_CTL_PIPE_SELECT_PIPE_C	(2 << 29)
#define CPU_BLC_PWM_DATA_BL_DUTY_CYC_MASK	(0xffff << 0)
#define PCH_BLC_PWM_CTL1_ENABLE			(1 << 31)
#define PCH_BLC_PWM_CTL1_BL_POLARITY_MASK	(1 << 29)
#define PCH_BLC_PWM_CTL1_PHASE_IN_INTR_STAT	(1 << 26)
#define PCH_BLC_PWM_CTL1_PHASE_IN_ENABLE	(1 << 25)
#define PCH_BLC_PWM_CTL1_PHASE_IN_INTR_EN	(1 << 24)
#define PCH_BLC_PWM_CTL1_PHASE_IN_TIME_BASE	(0xff << 16)
#define PCH_BLC_PWM_CTL1_PHASE_IN_COUNT		(0xff << 8)
#define PCH_BLC_PWM_CTL1_PHASE_IN_INCREMENT	(0xff << 0)
#define PCH_BLC_PWM_CTL2_BL_MOD_FREQ_MASK	(0xffff << 16)
#define PCH_BLC_PWM_CTL2_BL_DUTY_CYC_MASK	(0xffff << 0)

void Static_Init( void )
{
	Power_Cycle_Timer = Time.Now;
	Power_Up_Timer = Power_Cycle_Timer;
	Delays_US = Default_EDP_Delays_US;
}

void Check_PP_Delays(Panel_Power_Delays *Delays, Boolean *Override)
{
	for( int D = 0; D < 5; D++) {
		if(Delays[D] == 0) {
			Delays[D] = Default_EDP_Delays_US[D];
			*Override = True;
		}
	}
}

void Setup_PP_Sequencer(Boolean Default_Delays)
{
	Word32 Power_Delay, Port_Select;
	Boolean Override_Delays = False;

	Static_Init();
	if(Default_Delays) {
		Override_Delays = True;
	} else {
		Registers.Read(Registers.PCH_PP_ON_DELAYS, &Power_Delay);
		Delays_US[Power_Up_Delay] = 100 * Natural(((Power_Delay & PCH_PP_ON_DELAYS_PWR_UP_MASK) >> 16));
		Delays_US[Power_Up_To_BL_On] = 100 * Natural(Power_Delay & PCH_PP_ON_DELAYS_PWR_UP_BL_ON_MASK);
		Registers.Read(Registers.PCH_PP_OFF_DELAYS, &Power_Delay);
		Delays_US[Power_Down_Delay] = 100 * Natural(((Power_Delay & PCH_PP_OFF_DELAYS_PWR_DOWN_MASK) >> 16));
		Delays_US[BL_Off_To_Power_Down] = 100 * Natural(Power_Delay & PCH_PP_OFF_DELAYS_BL_OFF_PWR_DOWN_MASK);
		Registers.Read(Registers.PCH_PP_DIVISOR, &Power_Delay);
		if((Power_Delay & PCH_PP_DIVISOR_PWR_CYC_DELAY_MASK) > 1)
		{
			Delays_US[Power_Cycle_Delay] = 100000 * (Natural(Power_Delay && PCH_PP_DIVISOR_PWR_CYC_DELAY_MASK) - 1);
		}
		Check_PP_Delays(Delays_US, Override_Delays);
	}
	if(Override_Delays) {
		if(Config.Has_PP_Port_Select) {
			if(Config.Internal_Is_EDP) {
				Port_Select = PCH_PP_ON_DELAYS_PORT_SELECT_DP_A;
			} else {
				Port_Select = PCH_PP_ON_DELAYS_PORT_SELECT_LVDS;
			}
		} else {
			Port_Select = 0;
		}

		//  Force power-up to backlight-on delay to 100us as recommended by PRM.
		Registers.Unset_And_Set_Mask(Registers.PCH_PP_ON_DELAYS,
				PCH_PP_ON_DELAYS_PORT_SELECT_MASK |
				PCH_PP_ON_DELAYS_PWR_UP_MASK |
				PCH_PP_ON_DELAYS_PWR_UP_BL_ON_MASK,
				Port_Select |
				PCH_PP_ON_DELAYS_PWR_UP(Delays_US[Power_Up_Delay]) |
				PCH_PP_ON_DELAYS_PWR_UP_BL_ON(100)
		);
		Registers.Unset_And_Set_Mask(Registers.PCH_PP_OFF_DELAYS,
				PCH_PP_OFF_DELAYS_PWR_DOWN_MASK |
				PCH_PP_OFF_DELAYS_BL_OFF_PWR_DOWN_MASK,
				PCH_PP_OFF_DELAYS_PWR_DOWN(Delays_US[Power_Down_Delay]) |
				PCH_PP_OFF_DELAYS_BL_OFF_PWR_DOWN(Delays_US[BL_Off_To_Power_Down])
		);
		Registers.Unset_And_Set_Mask(Registers.PCH_PP_DIVISOR,
				PCH_PP_DIVISOR_PWR_CYC_DELAY_MASK,
				PCH_PP_DIVISOR_PWR_CYC_DELAY(Delays_US[Power_Cycle_Delay])
		);
	}

	if(Config.Has_PP_Write_Protection) {
		Registers.Unset_And_Set_Mask(Registers.PCH_PP_CONTROL,
				PCH_PP_CONTROL_WRITE_PROTECT_MASK,
				PCH_PP_CONTROL_WRITE_PROTECT_KEY |
				PCH_PP_CONTROL_POWER_DOWN_ON_RESET
		);
	} else {
		Registers.Set_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_POWER_DOWN_ON_RESET);
	}
}

void VDD_Override( void )
{
	//  Yeah, We could do, what we are supposed to do here. But OTOH, we
	//  are should wait for the full Power Up Delay, which we would have
	//  to do later again. And just powering on the display seems to work
	//  too. Also this function vanished on newer hardware.
	On();
}

void On(Boolean Wait)
{
	Boolean Was_On;

	Registers.Is_Set_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_TARGET_ON, &Was_On);
	if(!Was_On) {
		Time.Delay_Until(Power_Cycle_Timer);
	}
	Registers.Set_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_TARGET_ON);
	if(!Was_On) {
		Power_Up_Timer = Time.US_From_Now(Delays_US[Power_Up_Delay]);
	}
	if(Wait) {
		Wait_On();
	}
}

void Wait_On( void )
{
	Time.Delay_Until(Power_Up_Timer);
	Registers.Wait_Unset_Mask(Registers.PCH_PP_STATUS, PCH_PP_STATUS_PWR_SEQ_PROGRESS_MASK, 300);
	Registers.Unset_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_VDD_OVERRIDE);
}

void Off( void )
{
	Boolean Was_On;
	Registers.Is_Set_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_TARGET_ON, Was_On);
	Registers.Unset_Mask(Registers.PCH_PP_CONTROL,
			PCH_PP_CONTROL_TARGET_ON | PCH_PP_CONTROL_VDD_OVERRIDE);
	if(Was_On) {
		Time.U_Delay(Delays_US[Power_Down_Delay]);
	}
	Registers.Wait_Unset_Mask(Registers.PCH_PP_STATUS, PCH_PP_STATUS_PWR_SEQ_PROGRESS_MASK, 600);
	if(Was_On) {
		Power_Cycle_Timer = Time.US_From_Now(Delays_US[Power_Cycle_Delay]);
	}
}

void Backlight_On( void )
{
	Registers.Set_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_BACKLIGHT_ENABLE);
}

void Backlight_Off( void )
{
	Registers.Unset_Mask(Registers.PCH_PP_CONTROL, PCH_PP_CONTROL_BACKLIGHT_ENABLE);
}

void Set_Backlight(Word16 Level)
{
	Registers.Unset_And_Set_Mask(Registers.BLC_PWM_CPU_CTL, CPU_BLC_PWM_DATA_BL_DUTY_CYC_MASK, Word32(Level));
}

void Get_Max_Backlight(Word16 *Level)
{
	Word32 Reg;
	Registers.Read(Registers.BLC_PWM_PCH_CTL2, Reg);
	*Level = Word16((Reg & PCH_BLC_PWM_CTL2_BL_MOD_FREQ_MASK) >> 16);
}

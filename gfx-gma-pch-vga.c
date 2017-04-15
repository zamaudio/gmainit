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
#include "gfx-gma-pch-sideband.h"
#include "gfx-gma-pch-vga.h"

#define PCH_ADPA_DAC_ENABLE			(1 << 31)
#define PCH_ADPA_VSYNC_DISABLE			(1 << 11)
#define PCH_ADPA_HSYNC_DISABLE			(1 << 10)
#define PCH_ADPA_VSYNC_ACTIVE_HIGH		(1 << 4)
#define PCH_ADPA_HSYNC_ACTIVE_HIGH		(1 << 3)

#define PCH_ADPA_MASK \
		( PCH_TRANSCODER_SELECT_MASK | \
		PCH_ADPA_DAC_ENABLE | \
		PCH_ADPA_VSYNC_DISABLE | \
		PCH_ADPA_HSYNC_DISABLE | \
		PCH_ADPA_VSYNC_ACTIVE_HIGH | \
		PCH_ADPA_HSYNC_ACTIVE_HIGH )

void On(FDI_Port_Type Port, Mode_Type Mode)
{
	uint32_t Polarity = 0;
	if(Mode.H_Sync_Active_High) {
		Polarity |= PCH_ADPA_HSYNC_ACTIVE_HIGH;
	}
	if(Mode.V_Sync_Active_High) {
		Polarity |= PCH_ADPA_VSYNC_ACTIVE_HIGH;
	}
	Registers.Unset_And_Set_Mask(PCH_ADPA,
			PCH_ADPA_MASK,
			PCH_ADPA_DAC_ENABLE |
			PCH_TRANSCODER_SELECT[Port] |
			Polarity);
}

void Off( void )
{
	uint32_t Sync_Disable = 0;
	if(CONFIG_VGA_Has_Sync_Disable) {
		Sync_Disable = PCH_ADPA_HSYNC_DISABLE | PCH_ADPA_VSYNC_DISABLE;
	}
	Registers.Unset_And_Set_Mask(PCH_ADPA, PCH_ADPA_DAC_ENABLE, Sync_Disable);
}

#define PCH_PIXCLK_GATE_GATE			(0 << 0)
#define PCH_PIXCLK_GATE_UNGATE			(1 << 0)
#define SBI_SSCCTL_DISABLE			(1 << 0)
#define SBI_SSCDIVINTPHASE_DIVSEL_SHIFT		1
#define SBI_SSCDIVINTPHASE_DIVSEL_MASK		(0x7f << 1)
#define SBI_SSCDIVINTPHASE_INCVAL_SHIFT		8
#define SBI_SSCDIVINTPHASE_INCVAL_MASK		(0x7f << 8)
#define SBI_SSCDIVINTPHASE_DIR_SHIFT		15
#define SBI_SSCDIVINTPHASE_DIR_MASK		(1 << 15)
#define SBI_SSCDIVINTPHASE_PROPAGATE		(1 << 0)
#define SBI_SSCAUXDIV_FINALDIV2SEL_SHIFT	4
#define SBI_SSCAUXDIV_FINALDIV2SEL_MASK		(1 << 4)

uint32_t SBI_SSCDIVINTPHASE_DIVSEL(uint32_t Val)
{
	return (Val << SBI_SSCDIVINTPHASE_DIVSEL_SHIFT);
}

uint32_t SBI_SSCDIVINTPHASE_INCVAL(uint32_t Val)
{
	return (Val << SBI_SSCDIVINTPHASE_INCVAL_SHIFT);
}

uint32_t SBI_SSCDIVINTPHASE_DIR(uint32_t Val)
{
	return (Val << SBI_SSCDIVINTPHASE_DIR_SHIFT);
}

uint32_t SBI_SSCAUXDIV_FINALDIV2SEL(uint32_t Val)
{
	return (Val << SBI_SSCAUXDIV_FINALDIV2SEL_SHIFT);
}

void Clock_On(Mode_Type Mode)
{
	uint32_t Refclock = 2700000000;
	uint32_t Aux_Div, Div_Sel, Phase_Inc, Phase_Dir;

	Registers.Write(PCH_PIXCLK_GATE, PCH_PIXCLK_GATE_GATE);
	sideband_Set_Mask(SBI_ICLK, SBI_SSCCTL6, SBI_SSCCTL_DISABLE);
	Aux_Div = 0;
	Div_Sel = (uint32_t)(Refclock / Mode.Dotclock - 2);
	Phase_Inc = (uint32_t)((Refclock * 64) / Mode.Dotclock) & 0x0000003f;
	Phase_Dir = 0;
	sideband_Unset_And_Set_Mask(SBI_ICLK,
			SBI_SSCDIVINTPHASE6,
			SBI_SSCDIVINTPHASE_DIVSEL_MASK |
			SBI_SSCDIVINTPHASE_INCVAL_MASK |
			SBI_SSCDIVINTPHASE_DIR_MASK,
			SBI_SSCDIVINTPHASE_DIVSEL(Div_Sel) |
			SBI_SSCDIVINTPHASE_INCVAL(Phase_Inc) |
			SBI_SSCDIVINTPHASE_DIR(Phase_Dir) |
			SBI_SSCDIVINTPHASE_PROPAGATE);
	sideband_Unset_And_Set_Mask(SBI_ICLK,
			SBI_SSCAUXDIV,
			SBI_SSCAUXDIV_FINALDIV2SEL_MASK,
			SBI_SSCAUXDIV_FINALDIV2SEL(Aux_Div));
	sideband_Unset_Mask(SBI_ICLK, SBI_SSCCTL6, SBI_SSCCTL_DISABLE);
	Registers.Write(PCH_PIXCLK_GATE, PCH_PIXCLK_GATE_UNGATE);
}

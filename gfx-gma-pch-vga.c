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
	Word32 Polarity = 0;
	if(Mode.H_Sync_Active_High) {
		Polarity |= PCH_ADPA_HSYNC_ACTIVE_HIGH;
	}
	if(Mode.V_Sync_Active_High) {
		Polarity |= PCH_ADPA_VSYNC_ACTIVE_HIGH;
	}
	Registers.Unset_And_Set_Mask(Registers.PCH_ADPA,
			PCH_ADPA_MASK,
			PCH_ADPA_DAC_ENABLE |
			PCH_TRANSCODER_SELECT[Port] |
			Polarity);
}

void Off( void )
{
	Word32 Sync_Disable = 0;
	if(Config.VGA_Has_Sync_Disable) {
		Sync_Disable = PCH_ADPA_HSYNC_DISABLE | PCH_ADPA_VSYNC_DISABLE;
	}
	Registers.Unset_And_Set_Mask(Registers.PCH_ADPA, PCH_ADPA_DAC_ENABLE, Sync_Disable);
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

Word32 SBI_SSCDIVINTPHASE_DIVSEL(Word32 Val)
{
	return (Val << SBI_SSCDIVINTPHASE_DIVSEL_SHIFT);
}

Word32 SBI_SSCDIVINTPHASE_INCVAL(Word32 Val)
{
	return (Val << SBI_SSCDIVINTPHASE_INCVAL_SHIFT);
}

Word32 SBI_SSCDIVINTPHASE_DIR(Word32 Val)
{
	return (Val << SBI_SSCDIVINTPHASE_DIR_SHIFT);
}

Word32 SBI_SSCAUXDIV_FINALDIV2SEL(Word32 Val)
{
	return (Val << SBI_SSCAUXDIV_FINALDIV2SEL_SHIFT);
}

void Clock_On(Mode_Type Mode)
{
	Word32 Refclock = 2700000000;
	Word32 Aux_Div, Div_Sel, Phase_Inc, Phase_Dir;

	Registers.Write(Registers.PCH_PIXCLK_GATE, PCH_PIXCLK_GATE_GATE);
	Sideband.Set_Mask(Sideband.SBI_ICLK, Sideband.SBI_SSCCTL6, SBI_SSCCTL_DISABLE);
	Aux_Div = 0;
	Div_Sel = Word32(Refclock / Mode.Dotclock - 2);
	Phase_Inc = Word32((Refclock * 64) / Mode.Dotclock) & 0x0000003f;
	Phase_Dir = 0;
	Sideband.Unset_And_Set_Mask(Sideband.SBI_ICLK,
			Sideband.SBI_SSCDIVINTPHASE6,
			SBI_SSCDIVINTPHASE_DIVSEL_MASK |
			SBI_SSCDIVINTPHASE_INCVAL_MASK |
			SBI_SSCDIVINTPHASE_DIR_MASK,
			SBI_SSCDIVINTPHASE_DIVSEL(Div_Sel) |
			SBI_SSCDIVINTPHASE_INCVAL(Phase_Inc) |
			SBI_SSCDIVINTPHASE_DIR(Phase_Dir) |
			SBI_SSCDIVINTPHASE_PROPAGATE);
	Sideband.Unset_And_Set_Mask(Sideband.SBI_ICLK,
			Sideband.SBI_SSCAUXDIV,
			SBI_SSCAUXDIV_FINALDIV2SEL_MASK,
			SBI_SSCAUXDIV_FINALDIV2SEL(Aux_Div));
	Sideband.Unset_Mask(Sideband.SBI_ICLK, Sideband.SBI_SSCCTL6, SBI_SSCCTL_DISABLE);
	Registers.Write(Registers.PCH_PIXCLK_GATE, PCH_PIXCLK_GATE_UNGATE);
}

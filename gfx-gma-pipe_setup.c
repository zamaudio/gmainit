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
#include "gfx-gma-pipe_setup.h"

#define ILK_DISPLAY_CHICKEN1_VGA_MASK		(7 << 29)
#define ILK_DISPLAY_CHICKEN1_VGA_ENABLE		(5 << 29)
#define ILK_DISPLAY_CHICKEN2_VGA_MASK		(1 << 25)
#define ILK_DISPLAY_CHICKEN2_VGA_ENABLE		(0 << 25)
#define DSPCNTR_ENABLE				(1 << 31)
#define DSPCNTR_GAMMA_CORRECTION		(1 << 30)
#define DSPCNTR_DISABLE_TRICKLE_FEED		(1 << 14)
#define DSPCNTR_FORMAT_MASK			(15 << 26)
#define PLANE_CTL_PLANE_ENABLE			(1 << 31)
#define PLANE_CTL_SRC_PIX_FMT_RGB_32B_8888	(4 << 24)
#define PLANE_CTL_PLANE_GAMMA_DISABLE		(1 << 13)
#define PLANE_WM_ENABLE				(1 << 31)
#define PLANE_WM_LINES_SHIFT			14
#define PLANE_WM_LINES_MASK			(0x1f << 14)
#define PLANE_WM_BLOCKS_MASK			(0x3ff << 0)
#define VGA_SR_INDEX				0x3c4
#define VGA_SR_DATA				0x3c5
#define VGA_SR01				1
#define VGA_SR01_SCREEN_OFF			(1 << 5)
#define VGA_CONTROL_VGA_DISPLAY_DISABLE		(1 << 31)
#define VGA_CONTROL_BLINK_DUTY_CYCLE_MASK	(3 << 6)
#define VGA_CONTROL_BLINK_DUTY_CYCLE_50		(2 << 6)
#define VGA_CONTROL_VSYNC_BLINK_RATE_MASK	(0x3f << 0)

const uint32_t DSPCNTR_MASK =
		DSPCNTR_ENABLE |
		DSPCNTR_GAMMA_CORRECTION |
		DSPCNTR_FORMAT_MASK |
		DSPCNTR_DISABLE_TRICKLE_FEED;

typedef uint32_t VGA_Cycle_Count;

uint32_t VGA_CONTROL_VSYNC_BLINK_RATE(VGA_Cycle_Count Cycles)
{
	return (uint32_t)(Cycles) / 2 - 1;
}

#define PF_CTRL_ENABLE				(1 << 31)
#define PF_CTRL_PIPE_SELECT_MASK		(3 << 29)
#define PF_CTRL_FILTER_MED			(1 << 23)
#define PS_CTRL_ENABLE_SCALER			(1 << 31)
#define PS_CTRL_SCALER_MODE_7X5_EXTENDED	(1 << 28)
#define PS_CTRL_FILTER_SELECT_MEDIUM_2		(1 << 23)

uint32_t PLANE_WM_LINES(uint32_t Lines)
{
	return ((uint32_t)(Lines) << PLANE_WM_LINES_SHIFT) & PLANE_WM_LINES_MASK;
}

uint32_t PLANE_WM_BLOCKS(uint32_t Blocks)
{
	return (uint32_t)(Blocks) & PLANE_WM_BLOCKS_MASK;
}

uint32_t Encode(uint16_t LSW, uint16_t MSW)
{
	return (((uint32_t)(MSW) - 1) << 16) | ((uint32_t)(LSW) - 1);
}

void Clear_Watermarks(Controller_Type Controller)
{
	Registers.Write(Controller.PLANE_BUF_CFG, 0);
	for( int Level = 0; Level < 8; Level++)
	{
		Registers.Write(Controller.PLANE_WM[Level], 0);
	}

	Registers.Write(Controller.WM_LINETIME, 0);
}

void Setup_Watermarks(Controller_Type Controller)
{
	typedef uint32_t Per_Plane_Buffer_Range[3];
	const Per_Plane_Buffer_Range Buffer_Range = {
		(159 << 16) | 0,
		(319 << 16) | 160,
		(479 << 16) | 320
	};
	Registers.Write(Controller.PLANE_BUF_CFG, Buffer_Range[Controller.Pipe]);
	Registers.Write(Controller.PLANE_WM[0],
			PLANE_WM_ENABLE |
			PLANE_WM_LINES(2) |
			PLANE_WM_BLOCKS(160)
	);
}

void Setup_Hires_Plane(Controller_Type Controller, Framebuffer_Type Framebuffer)
{
	//  FIXME: setup correct format, based on framebuffer RGB format
	const uint32_t Format = 6 << 26;
	uint32_t PRI = DSPCNTR_ENABLE | Format;
	
	uint32_t To_Bytes(Width_Type Pixels)
	{
		return (uint32_t)((uint64_t)(Pixels) * 4 * Framebuffer.BPC / 8);
	}
	
	if(Config.Has_Plane_Control) {
		Registers.Write(Controller.PLANE_CTL,
				PLANE_CTL_PLANE_ENABLE |
				PLANE_CTL_SRC_PIX_FMT_RGB_32B_8888 |
				PLANE_CTL_PLANE_GAMMA_DISABLE);
		Registers.Write(Controller.PLANE_OFFSET, 0);
		Registers.Write(Controller.PLANE_SIZE,
			Encode((uint16_t)(Framebuffer.Width), (uint16_t)(Framebuffer.Height)));
		Registers.Write(Controller.PLANE_STRIDE,
			To_Bytes(Framebuffer.Stride) / 64);
		Registers.Write(Controller.PLANE_POS, 0);
		Registers.Write(Controller.PLANE_SURF, Framebuffer.Offset & 0xfffff000);
	} else {
		if(Config.Disable_Trickle_Feed) {
			PRI |= DSPCNTR_DISABLE_TRICKLE_FEED;
		}
		//  for now, just disable gamma LUT (can't do anything
		//  useful without colorimetry information from display)
		Registers.Unset_And_Set_Mask(Controller.DSPCNTR, DSPCNTR_MASK, PRI);
		Registers.Write(Controller.DSPSTRIDE, To_Bytes(Framebuffer.Stride));
		Registers.Write(Controller.DSPSURF, Framebuffer.Offset & 0xfffff000);
		if(Config.Has_DSP_Linoff) {
			Registers.Write(Controller.DSPLINOFF, 0);
		}
		Registers.Write(Controller.DSPTILEOFF, 0);
	}
}

void Setup_Display(Controller_Type Controller, Framebuffer_Type Framebuffer, BPC_Type Dither_BPC, bool Dither)
{
	uint8_t Reg8;

	if(Config.Has_Plane_Control) {
		Setup_Watermarks(Controller);
	}
	if(Framebuffer.Offset == VGA_PLANE_FRAMEBUFFER_OFFSET) {
		if(Config.VGA_Plane_Workaround) {
			Registers.Unset_And_Set_Mask(Registers.ILK_DISPLAY_CHICKEN1,
					ILK_DISPLAY_CHICKEN1_VGA_MASK,
					ILK_DISPLAY_CHICKEN1_VGA_ENABLE);
			Registers.Unset_And_Set_Mask(Registers.ILK_DISPLAY_CHICKEN2,
					ILK_DISPLAY_CHICKEN2_VGA_MASK,
					ILK_DISPLAY_CHICKEN2_VGA_ENABLE);
		}
		Registers.Unset_And_Set_Mask(Registers.VGACNTRL,
				VGA_CONTROL_VGA_DISPLAY_DISABLE |
				VGA_CONTROL_BLINK_DUTY_CYCLE_MASK |
				VGA_CONTROL_VSYNC_BLINK_RATE_MASK,
				VGA_CONTROL_BLINK_DUTY_CYCLE_50 |
				VGA_CONTROL_VSYNC_BLINK_RATE(30)
		);
		Port_IO.OutB(VGA_SR_INDEX, VGA_SR01);
		Reg8 = Port_IO.InB(VGA_SR_DATA);
		Port_IO.OutB(VGA_SR_DATA, Reg8 & ~VGA_SR01_SCREEN_OFF);
	} else {
		Setup_Hires_Plane(Controller, Framebuffer);
	}
	Registers.Write(Controller.PIPESRC,
			Encode((uint16_t)(Framebuffer.Height), (uint16_t)(Framebuffer.Width)));
	if(Config.Has_Pipeconf_Misc) {
		Registers.Write(Controller.PIPEMISC,
				Transcoder.BPC_Conf(Dither_BPC, Dither)
		);
	}
}

void Scale_Keep_Aspect(uint32_t *Width, uint32_t *Height, uint32_t Max_Width, uint32_t Max_Height, Framebuffer_Type Framebuffer)
{
	if((Max_Width * Framebuffer.Height) / Framebuffer.Width <= Max_Height) {
		*Width = Max_Width;
		*Height = (Max_Width * Framebuffer.Height) / Framebuffer.Width;
	} else {
		*Height = Max_Height;
		*Width = min(Max_Width, (Max_Height * Framebuffer.Width) / Framebuffer.Height);
	}
}

void Setup_Skylake_Pipe_Scaler(Controller_Type Controller, Mode_Type Mode, Framebuffer_Type Framebuffer)
{
	//  Enable 7x5 extended mode where possible:
	const uint32_t Scaler_Mode = PS_CTRL_SCALER_MODE_7X5_EXTENDED;
	//  We can scale up to 2.99x horizontally:
	const uint32_t Horizontal_Limit = ((Framebuffer.Width * 299) / 100);
	//  The third scaler is limited to 1.99x
	//  vertical scaling for source widths > 2048:
	const uint32_t Vertical_Limit = (Framebuffer.Height * 299) / 100;
	uint32_t Width, Height;
	//  Writes to WIN_SZ arm the PS registers.
	Scale_Keep_Aspect(&Width, &Height,
			min(Horizontal_Limit, (uint32_t)(Mode.H_Visible)),
			min(Vertical_Limit, (uint32_t)(Mode.V_Visible)),
			Framebuffer);
	Registers.Write(Controller.PS_CTRL_1, PS_CTRL_ENABLE_SCALER | Scaler_Mode);
	Registers.Write(Controller.PS_WIN_POS_1,
			(((uint32_t)((uint32_t)(Mode.H_Visible) - Width) / 2) << 16) |
			(uint32_t)((uint32_t)(Mode.V_Visible) - Height) / 2);
	Registers.Write(Controller.PS_WIN_SZ_1, ((uint32_t)(Width) << 16) | (uint32_t)(Height));
}

void Setup_Ironlake_Panel_Fitter(Controller_Type Controller, Mode_Type Mode, Framebuffer_Type Framebuffer)
{
	//  Force 1:1 mapping of panel fitter:pipe
	uint32_t PF_Ctrl_Pipe_Sel = 0;
	if (Config.Has_PF_Pipe_Select) {
		switch (Controller.PF_CTRL) {
		case Registers.PFA_CTL_1:
			PF_Ctrl_Pipe_Sel = 0 << 29;
			break;
		case Registers.PFB_CTL_1:
			PF_Ctrl_Pipe_Sel = 1 << 29;
			break;
		case Registers.PFC_CTL_1:
			PF_Ctrl_Pipe_Sel = 2 << 29;
			break;
		default:
			PF_Ctrl_Pipe_Sel = 0;
			break;
		}
	}

	uint32_t Width, Height;
	//  Writes to WIN_SZ arm the PF registers.
	Scale_Keep_Aspect(&Width, &Height, (uint32_t)(Mode.H_Visible), (uint32_t)(Mode.V_Visible), Framebuffer);
	Registers.Write(Controller.PF_CTRL,
			PF_CTRL_ENABLE |
			PF_Ctrl_Pipe_Sel |
			PF_CTRL_FILTER_MED);
	Registers.Write(Controller.PF_WIN_POS,
			(((uint32_t)((uint32_t)(Mode.H_Visible) - Width) / 2) << 16) |
			(uint32_t)((uint32_t)(Mode.V_Visible) - Height) / 2);
	Registers.Write(Controller.PF_WIN_SZ, ((uint32_t)(Width) << 16) | (uint32_t)(Height));
}

void Setup_Scaling(Controller_Type Controller, HW.GFX::Mode_Type Mode, HW.GFX::Framebuffer_Type Framebuffer)
{
	if((Framebuffer.Width != (uint32_t)(Mode.H_Visible)) || (Framebuffer.Height != (uint32_t)(Mode.V_Visible))) {
		if(Config.Has_Plane_Control) {
			Setup_Skylake_Pipe_Scaler(Controller, Mode, Framebuffer);
		} else {
			Setup_Ironlake_Panel_Fitter(Controller, Mode, Framebuffer);
		}
	}
}

void On(Pipe_Index Pipe, Port_Config Port_Cfg, Framebuffer_Type Framebuffer)
{
	//  Enable dithering if framebuffer BPC differs from port BPC,
	//  as smooth gradients look really bad without.
	const bool Dither = (Framebuffer.BPC != Port_Cfg.Mode.BPC);

	Transcoder.Setup(Pipe, Port_Cfg);
	Setup_Display(Controllers[Pipe], Framebuffer, Port_Cfg.Mode.BPC, Dither);
	Setup_Scaling(Controllers[Pipe], Port_Cfg.Mode, Framebuffer);
	Transcoder.On(Pipe, Port_Cfg, Dither);
}

void Planes_Off(Controller_Type Controller)
{
	Registers.Unset_Mask(Controller.SPCNTR, DSPCNTR_ENABLE);
	if(Config.Has_Plane_Control) {
		Clear_Watermarks(Controller);
		Registers.Unset_Mask(Controller.PLANE_CTL, PLANE_CTL_PLANE_ENABLE);
		Registers.Write(Controller.PLANE_SURF, 0);
	} else {
		Registers.Unset_Mask(Controller.DSPCNTR, DSPCNTR_ENABLE);
	}
}

void Panel_Fitter_Off(Controller_Type Controller)
{
	//  Writes to WIN_SZ arm the PS/PF registers.
	if(Config.Has_Plane_Control)
	{
		Registers.Unset_Mask(Controller.PS_CTRL_1, PS_CTRL_ENABLE_SCALER);
		Registers.Write(Controller.PS_WIN_SZ_1, 0);
		if((Controller.PS_CTRL_2 != Registers.INVALID_REGISTER) &&
				(Controller.PS_WIN_SZ_2 != Registers.INVALID_REGISTER)) {
			Registers.Unset_Mask(Controller.PS_CTRL_2, PS_CTRL_ENABLE_SCALER);
			Registers.Write(Controller.PS_WIN_SZ_2, 0);
		}
	} else {
		Registers.Unset_Mask(Controller.PF_CTRL, PF_CTRL_ENABLE);
		Registers.Write(Controller.PF_WIN_SZ, 0);
	}
}

void Off(Pipe_Index Pipe)
{
	Planes_Off(Controllers[Pipe]);
	Transcoder.Off(Pipe);
	Panel_Fitter_Off(Controllers[Pipe]);
	Transcoder.Clk_Off(Pipe);
}

void Legacy_VGA_Off( void )
{
	uint8_t Reg8;

	Port_IO.OutB(VGA_SR_INDEX, VGA_SR01);
	Reg8 = Port_IO.InB(VGA_SR_DATA);
	Port_IO.OutB(VGA_SR_DATA, Reg8 | VGA_SR01_SCREEN_OFF);
	//  PRM says 100us, Linux does 300
	udelay(100);
	Registers.Set_Mask(Registers.VGACNTRL, VGA_CONTROL_VGA_DISPLAY_DISABLE);
}

void All_Off( void )
{
	Legacy_VGA_Off();
	for( int Pipe = 0; Pipe < 3; Pipe++)
	{
		Planes_Off(Controllers[Pipe]);
		Transcoder.Off(Pipe);
		Panel_Fitter_Off(Controllers[Pipe]);
		Transcoder.Clk_Off(Pipe);
	}
}

void Update_Offset(Pipe_Index Pipe, Framebuffer_Type Framebuffer)
{
	Registers.Write(Controllers[Pipe].DSPSURF, Framebuffer.Offset & 0xfffff000);
}

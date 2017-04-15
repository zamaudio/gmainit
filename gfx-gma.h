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

#ifndef GFX_GMA_H
#define GFX_GMA_H

typedef enum {
	Ironlake,
	Sandybridge,
	Ivybridge,
	Haswell,
	Broadwell,
	Skylake
} CPU_Type;

typedef enum {
	Normal,
	ULT
} CPU_Variant;

typedef enum {
	Disabled,
	Internal,
	DP1,
	DP2,
	DP3,
	HDMI1,
	HDMI2,
	HDMI3,
	Analog
} Port_Type;

typedef struct t_Pipe_Config {
	Port_Type Port;
	Framebuffer_Type Framebuffer;
	Mode_Type Mode;
} Pipe_Config;

typedef enum {
	Primary,
	Secondary,
	Tertiary
} Pipe_Index;

typedef Pipe_Config Pipe_Configs[3];

// Special framebuffer offset to indicate legacy VGA plane.
// Only valid on primary pipe.
#define VGA_PLANE_FRAMEBUFFER_OFFSET 0xffffffff

Boolean Initialize(Word64 MMIO_Base, Word64 Write_Delay, Boolean Clean_State);
void Update_Outputs(Pipe_Configs Configs);
void Dump_Configs(Pipe_Configs Configs);
typedef int GTT_Address_Type;
typedef int GTT_Range;
void Write_GTT(GTT_Range GTT_Page, GTT_Address_Type Device_Address, Boolean Valid);
void Setup_Default_GTT(Framebuffer_Type FB, Word32 Phys_FB);

// --------------------------------------------------------------------------
//  State tracking for the currently configured pipes
Pipe_Configs Cur_Configs;

// --------------------------------------------------------------------------
//  Internal representation of a single pipe's configuration
typedef Port_Type Active_Port_Type;

typedef enum {
	DIGI_A,
	DIGI_B,
	DIGI_C,
	DIGI_D,
	DIGI_E
} GPU_Port;

typedef GPU_Port Digital_Port;

typedef enum {
	PCH_DAC,
	PCH_LVDS,
	PCH_HDMI_B,
	PCH_HDMI_C,
	PCH_HDMI_D,
	PCH_DP_B,
	PCH_DP_C,
	PCH_DP_D
} PCH_Port;

typedef PCH_Port PCH_HDMI_Port;
typedef PCH_Port PCH_DP_Port;

typedef struct t_Port_Config {
	GPU_Port Port;
	GMA.PCH_Port PCH_Port;
	Display_Type Display;
	Mode_Type Mode;
	Boolean Is_FDI;
	DP_Link FDI;
	DP_Link DP;
} Port_Config;

typedef enum {
	Simple_Training,
	Full_Training,
	Auto_Training
} FDI_Training_Type;

// --------------------------------------------------------------------------
typedef enum {
	DP_A,
	DP_B,
	DP_C,
	DP_D
} DP_Port;

#endif

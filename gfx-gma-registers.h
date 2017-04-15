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

#ifndef GFX_GMA_REGISTERS_H
#define GFX_GMA_REGISTERS_H

#include "gfx-gma.h"
#define INVALID_REGISTER		0x000000

// -------------------------------------------------------------------------
//  Pipe A registers
// -------------------------------------------------------------------------
//  pipe timing registers
#define HTOTAL_A 			0x060000
#define HBLANK_A 			0x060004
#define HSYNC_A 			0x060008
#define VTOTAL_A 			0x06000c
#define VBLANK_A 			0x060010
#define VSYNC_A 			0x060014
#define PIPEASRC 			0x06001c
#define PIPEACONF 			0x070008
#define PIPEAMISC 			0x070030
#define TRANS_HTOTAL_A 			0x0e0000
#define TRANS_HBLANK_A 			0x0e0004
#define TRANS_HSYNC_A 			0x0e0008
#define TRANS_VTOTAL_A 			0x0e000c
#define TRANS_VBLANK_A 			0x0e0010
#define TRANS_VSYNC_A 			0x0e0014
#define TRANSA_DATA_M1 			0x0e0030
#define TRANSA_DATA_N1 			0x0e0034
#define TRANSA_DP_LINK_M1 		0x0e0040
#define TRANSA_DP_LINK_N1 		0x0e0044
#define PIPEA_DATA_M1 			0x060030
#define PIPEA_DATA_N1 			0x060034
#define PIPEA_LINK_M1 			0x060040
#define PIPEA_LINK_N1 			0x060044
#define PIPEA_DDI_FUNC_CTL 		0x060400
#define PIPEA_MSA_MISC 			0x060410
       //  PCH sideband interface registers
#define SBI_ADDR 			0x0c6000
#define SBI_DATA 			0x0c6004
#define SBI_CTL_STAT 			0x0c6008
       //  clock registers
#define PCH_DPLL_A 			0x0c6014
#define PCH_PIXCLK_GATE 		0x0c6020
#define PCH_FPA0 			0x0c6040
#define PCH_FPA1 			0x0c6044
       //  panel fitter
#define PFA_CTL_1 			0x068080
#define PFA_WIN_POS 			0x068070
#define PFA_WIN_SZ 			0x068074
#define PS_WIN_POS_1_A 			0x068170
#define PS_WIN_SZ_1_A 			0x068174
#define PS_CTRL_1_A 			0x068180
#define PS_WIN_POS_2_A 			0x068270
#define PS_WIN_SZ_2_A 			0x068274
#define PS_CTRL_2_A 			0x068280
       //  display control
#define DSPACNTR 			0x070180
#define DSPALINOFF 			0x070184
#define DSPASTRIDE 			0x070188
#define PLANE_POS_1_A 			0x07018c
#define PLANE_SIZE_1_A 			0x070190
#define DSPASURF 			0x07019c
#define DSPATILEOFF 			0x0701a4
      //  sprite control
#define SPACNTR 			0x070280
       //  FDI and PCH transcoder control
#define FDI_TX_CTL_A 			0x060100
#define FDI_RXA_CTL 			0x0f000c
#define FDI_RX_MISC_A 			0x0f0010
#define FDI_RXA_IIR 			0x0f0014
#define FDI_RXA_IMR 			0x0f0018
#define FDI_RXA_TUSIZE1 		0x0f0030
#define TRANSACONF 			0x0f0008
#define TRANSA_CHICKEN2 		0x0f0064
       //  watermark registers
#define WM_LINETIME_A 			0x045270
#define PLANE_WM_1_A_0 			0x070240
#define PLANE_WM_1_A_1 			0x070244
#define PLANE_WM_1_A_2 			0x070248
#define PLANE_WM_1_A_3 			0x07024c
#define PLANE_WM_1_A_4 			0x070250
#define PLANE_WM_1_A_5 			0x070254
#define PLANE_WM_1_A_6 			0x070258
#define PLANE_WM_1_A_7 			0x07025c
#define PLANE_BUF_CFG_1_A 		0x07027c
       //  CPU transcoder clock select
#define TRANSA_CLK_SEL 			0x046140
// -------------------------------------------------------------------------
//  Pipe B registers
// -------------------------------------------------------------------------
//  pipe timing registers
#define HTOTAL_B 			0x061000
#define HBLANK_B 			0x061004
#define HSYNC_B 			0x061008
#define VTOTAL_B 			0x06100c
#define VBLANK_B 			0x061010
#define VSYNC_B 			0x061014
#define PIPEBSRC 			0x06101c
#define PIPEBCONF 			0x071008
#define PIPEBMISC 			0x071030
#define TRANS_HTOTAL_B 			0x0e1000
#define TRANS_HBLANK_B 			0x0e1004
#define TRANS_HSYNC_B 			0x0e1008
#define TRANS_VTOTAL_B 			0x0e100c
#define TRANS_VBLANK_B 			0x0e1010
#define TRANS_VSYNC_B 			0x0e1014
#define TRANSB_DATA_M1 			0x0e1030
#define TRANSB_DATA_N1 			0x0e1034
#define TRANSB_DP_LINK_M1 		0x0e1040
#define TRANSB_DP_LINK_N1 		0x0e1044
#define PIPEB_DATA_M1 			0x061030
#define PIPEB_DATA_N1 			0x061034
#define PIPEB_LINK_M1 			0x061040
#define PIPEB_LINK_N1 			0x061044
#define PIPEB_DDI_FUNC_CTL 		0x061400
#define PIPEB_MSA_MISC 			0x061410
      //  clock registers
#define PCH_DPLL_B 			0x0c6018
#define PCH_FPB0 			0x0c6048
#define PCH_FPB1 			0x0c604c
       //  panel fitter
#define PFB_CTL_1 			0x068880
#define PFB_WIN_POS 			0x068870
#define PFB_WIN_SZ 			0x068874
#define PS_WIN_POS_1_B 			0x068970
#define PS_WIN_SZ_1_B 			0x068974
#define PS_CTRL_1_B 			0x068980
#define PS_WIN_POS_2_B 			0x068a70
#define PS_WIN_SZ_2_B 			0x068a74
#define PS_CTRL_2_B 			0x068a80
      //  display control
#define DSPBCNTR 			0x071180
#define DSPBLINOFF 			0x071184
#define DSPBSTRIDE 			0x071188
#define PLANE_POS_1_B 			0x07118c
#define PLANE_SIZE_1_B 			0x071190
#define DSPBSURF 			0x07119c
#define DSPBTILEOFF 			0x0711a4
       //  sprite control
#define SPBCNTR 			0x071280
       //  FDI and PCH transcoder control
#define FDI_TX_CTL_B 			0x061100
#define FDI_RXB_CTL 			0x0f100c
#define FDI_RX_MISC_B 			0x0f1010
#define FDI_RXB_IIR 			0x0f1014
#define FDI_RXB_IMR 			0x0f1018
#define FDI_RXB_TUSIZE1 		0x0f1030
#define TRANSBCONF 			0x0f1008
#define TRANSB_CHICKEN2 		0x0f1064
       //  watermark registers
#define WM_LINETIME_B 			0x045274
#define PLANE_WM_1_B_0 			0x071240
#define PLANE_WM_1_B_1 			0x071244
#define PLANE_WM_1_B_2 			0x071248
#define PLANE_WM_1_B_3 			0x07124c
#define PLANE_WM_1_B_4 			0x071250
#define PLANE_WM_1_B_5 			0x071254
#define PLANE_WM_1_B_6 			0x071258
#define PLANE_WM_1_B_7 			0x07125c
#define PLANE_BUF_CFG_1_B 		0x07127c
       //  CPU transcoder clock select
#define TRANSB_CLK_SEL 			0x046144
       // ------------------------------------------------------------------------
      //  Pipe C registers
      // ------------------------------------------------------------------------
      //  pipe timing registers
#define HTOTAL_C 			0x062000
#define HBLANK_C 			0x062004
#define HSYNC_C 			0x062008
#define VTOTAL_C 			0x06200c
#define VBLANK_C 			0x062010
#define VSYNC_C 			0x062014
#define PIPECSRC 			0x06201c
#define PIPECCONF 			0x072008
#define PIPECMISC 			0x072030
#define TRANS_HTOTAL_C 			0x0e2000
#define TRANS_HBLANK_C 			0x0e2004
#define TRANS_HSYNC_C 			0x0e2008
#define TRANS_VTOTAL_C 			0x0e200c
#define TRANS_VBLANK_C 			0x0e2010
#define TRANS_VSYNC_C 			0x0e2014
#define TRANSC_DATA_M1 			0x0e2030
#define TRANSC_DATA_N1 			0x0e2034
#define TRANSC_DP_LINK_M1 		0x0e2040
#define TRANSC_DP_LINK_N1 		0x0e2044
#define PIPEC_DATA_M1 			0x062030
#define PIPEC_DATA_N1 			0x062034
#define PIPEC_LINK_M1 			0x062040
#define PIPEC_LINK_N1 			0x062044
#define PIPEC_DDI_FUNC_CTL 		0x062400
#define PIPEC_MSA_MISC 			0x062410
       //  panel fitter
#define PFC_CTL_1 			0x069080
#define PFC_WIN_POS 			0x069070
#define PFC_WIN_SZ 			0x069074
#define PS_WIN_POS_1_C 			0x069170
#define PS_WIN_SZ_1_C 			0x069174
#define PS_CTRL_1_C 			0x069180
       //  display control
#define DSPCCNTR 			0x072180
#define DSPCLINOFF 			0x072184
#define DSPCSTRIDE 			0x072188
#define PLANE_POS_1_C 			0x07218c
#define PLANE_SIZE_1_C 			0x072190
#define DSPCSURF 			0x07219c
#define DSPCTILEOFF 			0x0721a4
      //  sprite control
#define SPCCNTR 			0x072280
       //  PCH transcoder control
#define FDI_TX_CTL_C 			0x062100
#define FDI_RXC_CTL 			0x0f200c
#define FDI_RX_MISC_C 			0x0f2010
#define FDI_RXC_IIR 			0x0f2014
#define FDI_RXC_IMR 			0x0f2018
#define FDI_RXC_TUSIZE1 		0x0f2030
#define TRANSCCONF 			0x0f2008
#define TRANSC_CHICKEN2 		0x0f2064
      //  watermark registers
#define WM_LINETIME_C 			0x045278
#define PLANE_WM_1_C_0 			0x072240
#define PLANE_WM_1_C_1 			0x072244
#define PLANE_WM_1_C_2 			0x072248
#define PLANE_WM_1_C_3 			0x07224c
#define PLANE_WM_1_C_4 			0x072250
#define PLANE_WM_1_C_5 			0x072254
#define PLANE_WM_1_C_6 			0x072258
#define PLANE_WM_1_C_7 			0x07225c
#define PLANE_BUF_CFG_1_C 		0x07227c
       //  CPU transcoder clock select
#define TRANSC_CLK_SEL 			0x046148
// ------------------------------------------------------------------------
//  Pipe EDP registers
// ------------------------------------------------------------------------
//  pipe timing registers
#define HTOTAL_EDP 			0x06f000
#define HBLANK_EDP 			0x06f004
#define HSYNC_EDP 			0x06f008
#define VTOTAL_EDP 			0x06f00c
#define VBLANK_EDP 			0x06f010
#define VSYNC_EDP 			0x06f014
#define PIPE_EDP_CONF 			0x07f008
#define PIPE_EDP_DATA_M1 		0x06f030
#define PIPE_EDP_DATA_N1 		0x06f034
#define PIPE_EDP_LINK_M1 		0x06f040
#define PIPE_EDP_LINK_N1 		0x06f044
#define PIPE_EDP_DDI_FUNC_CTL 		0x06f400
#define PIPE_EDP_MSA_MISC 		0x06f410
       //  PSR registers
#define SRD_CTL 			0x064800
#define SRD_CTL_A 			0x060800
#define SRD_CTL_B 			0x061800
#define SRD_CTL_C 			0x062800
#define SRD_CTL_EDP 			0x06f800
#define SRD_STATUS 			0x064840
#define SRD_STATUS_A 			0x060840
#define SRD_STATUS_B 			0x061840
#define SRD_STATUS_C 			0x062840
#define SRD_STATUS_EDP 			0x06f840
       //  DDI registers
      //  aliased by DP_CTL_A
#define DDI_BUF_CTL_A 			0x064000
       //  aliased by DP_AUX_CTL_A
#define DDI_AUX_CTL_A 			0x064010
       //  aliased by DP_AUX_DATA_A_1
#define DDI_AUX_DATA_A_1 		0x064014
       //  aliased by DP_AUX_DATA_A_2
#define DDI_AUX_DATA_A_2 		0x064018
       //  aliased by DP_AUX_DATA_A_3
#define DDI_AUX_DATA_A_3 		0x06401c
       //  aliased by DP_AUX_DATA_A_4
#define DDI_AUX_DATA_A_4 		0x064020
       //  aliased by DP_AUX_DATA_A_5
#define DDI_AUX_DATA_A_5 		0x064024
#define DDI_AUX_MUTEX_A 		0x06402c
#define DDI_BUF_CTL_B 			0x064100
#define DDI_AUX_CTL_B 			0x064110
#define DDI_AUX_DATA_B_1 		0x064114
#define DDI_AUX_DATA_B_2 		0x064118
#define DDI_AUX_DATA_B_3 		0x06411c
#define DDI_AUX_DATA_B_4 		0x064120
#define DDI_AUX_DATA_B_5 		0x064124
#define DDI_AUX_MUTEX_B 		0x06412c
#define DDI_BUF_CTL_C 			0x064200
#define DDI_AUX_CTL_C 			0x064210
#define DDI_AUX_DATA_C_1 		0x064214
#define DDI_AUX_DATA_C_2 		0x064218
#define DDI_AUX_DATA_C_3 		0x06421c
#define DDI_AUX_DATA_C_4 		0x064220
#define DDI_AUX_DATA_C_5 		0x064224
#define DDI_AUX_MUTEX_C 		0x06422c
#define DDI_BUF_CTL_D 			0x064300
#define DDI_AUX_CTL_D 			0x064310
#define DDI_AUX_DATA_D_1 		0x064314
#define DDI_AUX_DATA_D_2 		0x064318
#define DDI_AUX_DATA_D_3 		0x06431c
#define DDI_AUX_DATA_D_4 		0x064320
#define DDI_AUX_DATA_D_5 		0x064324
#define DDI_AUX_MUTEX_D 		0x06432c
#define DDI_BUF_CTL_E 			0x064400
#define DP_TP_CTL_A 			0x064040
#define DP_TP_CTL_B 			0x064140
#define DP_TP_CTL_C 			0x064240
#define DP_TP_CTL_D 			0x064340
#define DP_TP_CTL_E 			0x064440
#define DP_TP_STATUS_B 			0x064144
#define DP_TP_STATUS_C 			0x064244
#define DP_TP_STATUS_D 			0x064344
#define DP_TP_STATUS_E 			0x064444
#define PORT_CLK_SEL_DDIA 		0x046100
#define PORT_CLK_SEL_DDIB 		0x046104
#define PORT_CLK_SEL_DDIC 		0x046108
#define PORT_CLK_SEL_DDID 		0x04610c
#define PORT_CLK_SEL_DDIE 		0x046110
       //  Skylake DPLL registers
#define DPLL1_CFGR1 			0x06c040
#define DPLL1_CFGR2 			0x06c044
#define DPLL2_CFGR1 			0x06c048
#define DPLL2_CFGR2 			0x06c04c
#define DPLL3_CFGR1 			0x06c050
#define DPLL3_CFGR2 			0x06c054
#define DPLL_CTRL1 			0x06c058
#define DPLL_CTRL2 			0x06c05c
#define DPLL_STATUS 			0x06c060
       //  CD CLK register
#define CDCLK_CTL 			0x046000
       //  Skylake LCPLL registers
#define LCPLL1_CTL 			0x046010
#define LCPLL2_CTL 			0x046014
       //  SPLL register
#define SPLL_CTL 			0x046020
       //  WRPLL registers
#define WRPLL_CTL_1 			0x046040
#define WRPLL_CTL_2 			0x046060
       //  Power Down Well registers
#define PWR_WELL_CTL_BIOS 		0x045400
#define PWR_WELL_CTL_DRIVER 		0x045404
#define PWR_WELL_CTL_KVMR 		0x045408
#define PWR_WELL_CTL_DEBUG 		0x04540c
#define PWR_WELL_CTL5 			0x045410
#define PWR_WELL_CTL6 			0x045414
       //  class Panel registers
#define PCH_PP_STATUS 			0x0c7200
#define PCH_PP_CONTROL 			0x0c7204
#define PCH_PP_ON_DELAYS 		0x0c7208
#define PCH_PP_OFF_DELAYS 		0x0c720c
#define PCH_PP_DIVISOR 			0x0c7210
#define BLC_PWM_CPU_CTL 		0x048254
#define BLC_PWM_PCH_CTL2 		0x0c8254
       //  PCH LVDS Connector Registers
#define PCH_LVDS 			0x0e1180
       //  PCH ADPA Connector Registers
#define PCH_ADPA 			0x0e1100
       //  PCH HDMIB Connector Registers
#define PCH_HDMIB 			0x0e1140
       //  PCH HDMIC Connector Registers
#define PCH_HDMIC 			0x0e1150
       //  PCH HDMID Connector Registers
#define PCH_HDMID 			0x0e1160
       //  Intel Registers
#define VGACNTRL 			0x041000
#define FUSE_STATUS 			0x042000
#define FBA_CFB_BASE 			0x043200
#define IPS_CTL 			0x043408
#define ARB_CTL 			0x045000
#define DBUF_CTL 			0x045008
#define NDE_RSTWRN_OPT 			0x046408
#define PCH_DREF_CONTROL 		0x0c6200
#define BLC_PWM_PCH_CTL1 		0x0c8250
#define BLC_PWM_CPU_CTL2 		0x048250
#define PCH_DPLL_SEL 			0x0c7000
#define GT_MAILBOX 			0x138124
#define GT_MAILBOX_DATA 		0x138128
#define GT_MAILBOX_DATA_1 		0x13812c
#define PCH_DP_B 			0x0e4100
#define PCH_DP_AUX_CTL_B 		0x0e4110
#define PCH_DP_AUX_DATA_B_1 		0x0e4114
#define PCH_DP_AUX_DATA_B_2 		0x0e4118
#define PCH_DP_AUX_DATA_B_3 		0x0e411c
#define PCH_DP_AUX_DATA_B_4 		0x0e4120
#define PCH_DP_AUX_DATA_B_5 		0x0e4124
#define PCH_DP_C 			0x0e4200
#define PCH_DP_AUX_CTL_C 		0x0e4210
#define PCH_DP_AUX_DATA_C_1 		0x0e4214
#define PCH_DP_AUX_DATA_C_2 		0x0e4218
#define PCH_DP_AUX_DATA_C_3 		0x0e421c
#define PCH_DP_AUX_DATA_C_4 		0x0e4220
#define PCH_DP_AUX_DATA_C_5 		0x0e4224
#define PCH_DP_D 			0x0e4300
#define PCH_DP_AUX_CTL_D 		0x0e4310
#define PCH_DP_AUX_DATA_D_1 		0x0e4314
#define PCH_DP_AUX_DATA_D_2 		0x0e4318
#define PCH_DP_AUX_DATA_D_3 		0x0e431c
#define PCH_DP_AUX_DATA_D_4 		0x0e4320
#define PCH_DP_AUX_DATA_D_5 		0x0e4324
       //  watermark registers
#define WM1_LP_ILK 			0x045108
#define WM2_LP_ILK 			0x04510c
#define WM3_LP_ILK 			0x045110
       //  audio VID/DID
#define AUD_VID_DID 			0x065020
#define PCH_AUD_VID_DID 		0x0e5020
       //  interrupt registers
#define DEISR				0x044000
#define DEIMR				0x044004
#define DEIIR				0x044008
#define DEIER				0x04400c
#define GTISR				0x044010
#define GTIMR				0x044014
#define GTIIR				0x044018
#define GTIER				0x04401c
#define SDEISR				0x0c4000
#define SDEIMR				0x0c4004
#define SDEIIR				0x0c4008
#define SDEIER				0x0c400c
       //  I2C stuff
#define PCH_GMBUS0			0x0c5100
#define PCH_GMBUS1			0x0c5104
#define PCH_GMBUS2			0x0c5108
#define PCH_GMBUS3			0x0c510c
#define PCH_GMBUS4			0x0c5110
#define PCH_GMBUS5			0x0c5120
       //  clock gating -- maybe have to touch this
#define DSPCLK_GATE_D			0x042020
#define PCH_FDI_CHICKEN_B_C		0x0c2000
#define PCH_DSPCLK_GATE_D		0x0c2020
       //  hotplug and initial detection
#define HOTPLUG_CTL 			0x044030
#define SHOTPLUG_CTL 			0x0c4030
#define SFUSE_STRAP 			0x0c2014
       //  Render Engine Command Streamer
#define ARB_MODE 			0x004030
#define HWS_PGA 			0x004080
#define RCS_RING_BUFFER_TAIL		0x002030
#define VCS_RING_BUFFER_TAIL		0x012030
#define BCS_RING_BUFFER_TAIL		0x022030
#define RCS_RING_BUFFER_HEAD		0x002034
#define VCS_RING_BUFFER_HEAD		0x012034
#define BCS_RING_BUFFER_HEAD		0x022034
#define RCS_RING_BUFFER_STRT		0x002038
#define VCS_RING_BUFFER_STRT		0x012038
#define BCS_RING_BUFFER_STRT		0x022038
#define RCS_RING_BUFFER_CTL		0x00203c
#define VCS_RING_BUFFER_CTL		0x01203c
#define BCS_RING_BUFFER_CTL		0x02203c
#define MI_MODE 			0x00209c
#define INSTPM 				0x0020c0
#define GAB_CTL_REG 			0x024000
#define PP_DCLV_HIGH 			0x002220
#define PP_DCLV_LOW 			0x002228
#define VCS_PP_DCLV_HIGH 		0x012220
#define VCS_PP_DCLV_LOW 		0x012228
#define BCS_PP_DCLV_HIGH 		0x022220
#define BCS_PP_DCLV_LOW 		0x022228
#define ILK_DISPLAY_CHICKEN2 		0x042004
#define UCGCTL1 			0x009400
#define UCGCTL2 			0x009404
#define MBCTL 				0x00907c
#define HWSTAM 				0x002098
#define VCS_HWSTAM 			0x012098
#define BCS_HWSTAM 			0x022098
#define IIR 				0x044028
#define PIPE_FRMCNT_A 			0x070040
#define PIPE_FRMCNT_B 			0x071040
#define PIPE_FRMCNT_C 			0x072040
#define FBC_CTL 			0x043208
#define PIPE_VSYNCSHIFT_A 		0x060028
#define PIPE_VSYNCSHIFT_B 		0x061028
#define PIPE_VSYNCSHIFT_C 		0x062028
#define WM_PIPE_A 			0x045100
#define WM_PIPE_B 			0x045104
#define WM_PIPE_C 			0x045200
#define PIPE_SCANLINE_A 		0x070000
#define PIPE_SCANLINE_B 		0x071000
#define PIPE_SCANLINE_C 		0x072000
#define GFX_MODE 			0x002520
#define CACHE_MODE_0 			0x002120
#define SLEEP_PSMI_CONTROL 		0x012050
#define CTX_SIZE 			0x0021a0
#define GAC_ECO_BITS 			0x014090
#define GAM_ECOCHK 			0x004090
#define QUIRK_02084 			0x002084
#define QUIRK_02090 			0x002090
#define GT_MODE 			0x0020d0
#define QUIRK_F0060 			0x0f0060
#define QUIRK_F1060 			0x0f1060
#define QUIRK_F2060			0x0f2060
#define AUD_CNTRL_ST2			0x0e50c0
#define AUD_CNTL_ST_A			0x0e50b4
#define AUD_CNTL_ST_B			0x0e51b4
#define AUD_CNTL_ST_C			0x0e52b4
#define AUD_HDMIW_HDMIEDID_A		0x0e5050
#define AUD_HDMIW_HDMIEDID_B		0x0e5150
#define AUD_HDMIW_HDMIEDID_C		0x0e5250
#define AUD_CONFIG_A			0x0e5000
#define AUD_CONFIG_B			0x0e5100
#define AUD_CONFIG_C			0x0e5200
#define TRANS_DP_CTL_A			0x0e0300
#define TRANS_DP_CTL_B			0x0e1300
#define TRANS_DP_CTL_C			0x0e2300
#define TRANS_VSYNCSHIFT_A		0x0e0028
#define TRANS_VSYNCSHIFT_B		0x0e1028
#define TRANS_VSYNCSHIFT_C		0x0e2028
#define PCH_RAWCLK_FREQ			0x0c6204
#define QUIRK_C2004 			0x0c2004

#define Default_Timeout_MS		10

typedef uint32_t Registers_Index;
//  aliased registers
#define DP_CTL_A			DDI_BUF_CTL_A
#define DP_AUX_CTL_A			DDI_AUX_CTL_A
#define DP_AUX_DATA_A_1			DDI_AUX_DATA_A_1
#define DP_AUX_DATA_A_2			DDI_AUX_DATA_A_2
#define DP_AUX_DATA_A_3			DDI_AUX_DATA_A_3
#define DP_AUX_DATA_A_4			DDI_AUX_DATA_A_4
#define DP_AUX_DATA_A_5			DDI_AUX_DATA_A_5
#define ILK_DISPLAY_CHICKEN1		FUSE_STATUS

void Posting_Read(Registers_Index Register);
void Read(Registers_Index Register, uint32_t *Value, bool Verbose);
void Write(Registers_Index Register, uint32_t Value);
void Is_Set_Mask(Registers_Index Register, uint32_t Mask, bool *Result);
void Wait(Registers_Index Register, uint32_t Mask, uint32_t Value, uint32_t TOut_MS, bool Verbose);
void Wait_Set_Mask(Registers_Index Register, uint32_t Mask, uint32_t TOut_MS, bool Verbose);
void Wait_Unset_Mask(Registers_Index Register, uint32_t Mask, uint32_t TOut_MS, bool Verbose);
void Set_Mask(Registers_Index Register, uint32_t Mask);
void Unset_Mask(Registers_Index Register, uint32_t Mask);
void Unset_And_Set_Mask(Registers_Index Register, uint32_t Mask_Unset, uint32_t Mask_Set);
void Write_GTT(GTT_Range GTT_Page, GTT_Address_Type Device_Address, bool Valid);
void Set_Register_Base(uint64_t Base);

#endif

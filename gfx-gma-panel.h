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

#ifndef GFX_GMA_PANEL_H
#define GFX_GMA_PANEL_H

#include <stdbool.h>
#include <inttypes.h>
#include "gfx-gma-registers.h"

void Static_Init( void );
void Setup_PP_Sequencer(bool Default_Delays);
void VDD_Override( void );
void On(bool Wait);
void Wait_On( void );
void Off( void );
void Backlight_On( void );
void Backlight_Off( void );
void Set_Backlight(uint16_t Level);
void Get_Max_Backlight(uint16_t *Level);

#endif

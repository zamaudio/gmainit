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

#ifndef GFX_GMA_CONNECTOR_INFO_H
#define GFX_GMA_CONNECTOR_INFO_H

#include "gfx-gma.h"

bool Preferred_Link_Setting(Port_Config *Port_Cfg);
bool Next_Link_Setting(Port_Config *Port_Cfg);
uint8_t Default_BPC(Port_Config Port_Cfg);

#endif

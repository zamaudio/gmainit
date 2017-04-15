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

#ifndef GFX_GMA_I2C_H
#define GFX_GMA_I2C_H

#include <stdbool.h>
#include "gfx-i2c.h"
#include "gfx-gma.h"

bool I2C_Read(PCH_Port Port, Transfer_Address Address, Transfer_Length *Length, Transfer_Data *Data);

#endif

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

#include <inttypes.h>
#include "gfx.h"

void Fill(uint64_t Linear_FB, Framebuffer_Type Framebuffer)
{
	int32_t Line_Start = 0;
	for( int Line = 0 ; Line <= Framebuffer.Height - 1 ; Line++) {
		for( int Col = 0 ; Col <= Framebuffer.Width - 1 ; Col++) {
			Write(Linear_FB + Line_Start + Col, 0xFF000000);
		}
		Line_Start = Line_Start + Framebuffer.Stride;
	}
}

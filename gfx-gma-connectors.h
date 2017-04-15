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

#ifndef GFX_GMA_CONNECTORS_H
#define GFX_GMA_CONNECTORS_H

Boolean Pre_On(Pipe_Index Pipe, Port_Config Port_Cfg, Word32 PLL_Hint);
Boolean Post_On(Port_Config Port_Cfg, Word32 PLL_Hint);
void Pre_Off(Port_Config Port_Cfg);
void Post_Off(Port_Config Port_Cfg);
void Pre_All_Off( void );
void Post_All_Off( void );

#endif

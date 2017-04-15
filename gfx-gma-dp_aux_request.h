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

#ifndef GFX_GMA_DP_AUX_REQUEST_H
#define GFX_GMA_DP_AUX_REQUEST_H

#include <stdbool.h>
#include "gfx-dp_defs.h"

bool Do_Aux_Request(DP_Port Port, Aux_Request Request, Aux_Request_Length Request_Length, Aux_Response *Response, Aux_Response_Length *Response_Length);

#endif

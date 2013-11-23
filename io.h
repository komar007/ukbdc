/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "platforms.h"
#ifdef PLATFORM_alpha
	#include "hc595.h"
#endif

#include <avr/io.h>
#include <stdbool.h>

#define OUTPUT	true
#define INPUT	false

bool IO_get(uint8_t pin);
void IO_set(uint8_t pin, bool val);
void IO_config(uint8_t pin, bool dir);

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

/*! \file io.h
 * This module creates a layer of abstraction for microcontroller pins
 * Every pin is represented by a 1-byte unsigned number. Numbers up to 127
 * represent *internal* pins, which means pins belonging to microcontroller's
 * ports. Numbers higher than that represent *external* pins. The external
 * pins can be anything and the implementation of the writing, reading and
 * configuring functions can be provided by the user.
 */

#pragma once

#include "platforms.h"
#ifdef PLATFORM_alpha
	#include "hc595.h"
#endif

#include <avr/io.h>
#include <stdbool.h>

/*! A macro for pin's output mode */
#define OUTPUT	true
/*! A macro for pin's input mode */
#define INPUT	false

/*! Reads the state of pin
 * \param pin pin number
 * \return binary state of the pin
 */
bool IO_get(uint8_t pin);
/*! Sets the state of pin
 * \param pin pin number
 * \oaran val the value to set
 */
void IO_set(uint8_t pin, bool val);
/*! Sets the mode of a pin
 * \param pin pin number
 * \param dir the mode of the pin (\ref INPUT or \ref OUTPUT)
 */
void IO_config(uint8_t pin, bool dir);

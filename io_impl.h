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

#include <avr/io.h>
#include <stdbool.h>

static inline bool IO_get_internal(uint8_t pin)
{
	return (*PINS[pin].pinx & PINS[pin].mask) ? true : false;
}

static inline void IO_set_internal(uint8_t pin, bool val)
{
	if (val)
		*PINS[pin].portx |= PINS[pin].mask;
	else
		*PINS[pin].portx &= ~PINS[pin].mask;
}

static inline void IO_config_internal(uint8_t pin, bool dir)
{
	if (dir == OUTPUT)
		*PINS[pin].ddrx |= PINS[pin].mask;
	else
		*PINS[pin].ddrx &= ~PINS[pin].mask;
}


#if defined(PLATFORM_ikea) || defined(PLATFORM_alpha) || defined(PLATFORM_gh60) || defined(PLATFORM_gh60b) || defined(PLATFORM_ghpad)
static inline bool IO_get_external(__attribute__((unused)) uint8_t pin)
{
	return false;
}
#endif

#if defined(PLATFORM_ikea) || defined(PLATFORM_gh60) || defined(PLATFORM_gh60b) || defined(PLATFORM_ghpad)
static inline void IO_set_external(__attribute__((unused)) uint8_t pin, __attribute__((unused)) bool val)
{
}
#elif defined(PLATFORM_alpha)
void IO_set_external(uint8_t pin, bool val)
{
	static uint16_t hc_state = 0;
	if (val)
		hc_state |= 0x0001 << (pin & 0x7f);
	else
		hc_state &= ~(0x0001 << (pin & 0x7f));
	HC595_set_outputs(hc_state);
}
#endif


#if defined(PLATFORM_alpha) || defined(PLATFORM_ikea) || defined(PLATFORM_gh60) || defined(PLATFORM_gh60b) || defined(PLATFORM_ghpad)
static inline void IO_config_external(__attribute__((unused)) uint8_t pin, __attribute__((unused)) bool dir)
{
}
#endif

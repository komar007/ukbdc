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

#define F_CPU 16000000

#include <stdint.h>
#include <avr/io.h>
#include "leds.h"

#ifdef PLATFORM_alpha
	#define DATAFLASH_PAGE_SIZE	528
	#define DATAFLASH_CS_SET()	PORTB &= ~_BV(PB0)
	#define DATAFLASH_CS_CLEAR()	PORTB |= _BV(PB0)
	#define DATAFLASH_SI_HIGH()	PORTB |= _BV(PB2)
	#define DATAFLASH_SI_LOW()	PORTB &= ~_BV(PB2)
	#define DATAFLASH_CLK_HIGH()	PORTB |= _BV(PB1)
	#define DATAFLASH_CLK_LOW()	PORTB &= ~_BV(PB1)
	#define DATAFLASH_SO_STATE()	((PINB >> PB3) & 0x01)

	#define HC595_BUFEN_SET()	PORTC &= ~_BV(PC6)
	#define HC595_BUFEN_CLEAR()	PORTC |= ~_BV(PC6)
	#define HC595_SER_HIGH()	PORTB |= _BV(PB2)
	#define HC595_SER_LOW()		PORTB &= ~_BV(PB2)
	#define HC595_CLK_HIGH()	PORTB |= _BV(PB1)
	#define HC595_CLK_LOW()		PORTB &= ~_BV(PB1)
	#define HC595_LATCH_HIGH()	PORTC |= _BV(PC7)
	#define HC595_LATCH_LOW()	PORTC &= ~_BV(PC7)
#endif

#ifdef PLATFORM_alpha
	#define NUM_IO			19
	#define NUM_LEDS		0
#elif defined(PLATFORM_gh60) | defined(PLATFORM_gh60b)
	#define NUM_IO			20
	#define NUM_LEDS		2
#elif defined(PLATFORM_ghpad)
	#define NUM_IO			10
	#define NUM_LEDS		2
#else
	#define NUM_IO			1
	#define NUM_LEDS		0
#endif

struct pin_config {
	volatile uint8_t *portx;
	volatile uint8_t *pinx;
	volatile uint8_t *ddrx;
	uint8_t mask;
};

struct pin_config PINS[NUM_IO];
volatile struct led leds[NUM_LEDS];

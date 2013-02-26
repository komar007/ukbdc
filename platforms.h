#pragma once

#define F_CPU 16000000

#include <stdint.h>
#include <avr/io.h>

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
#elif defined(PLATFORM_gh60)
	#define NUM_IO			20
#elif defined(PLATFORM_ghpad)
	#define NUM_IO			10
#else
	#define NUM_IO			0
#endif

struct pin_config {
	volatile uint8_t *portx;
	volatile uint8_t *pinx;
	volatile uint8_t *ddrx;
	uint8_t mask;
};

struct pin_config PINS[NUM_IO];

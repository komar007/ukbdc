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
#endif

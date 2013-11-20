#include "timer.h"
#include "aux.h"
#include "system.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void TIMER_init()
{
	/* set up timer 0 to no prescale */
	TCCR1A = 0x00;
	TCCR1B = 0x01;
	OCR1A = CYCLES_BETWEEN_TICKS;
	TIMSK1 = _BV(OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	static uint8_t cnt = 0;
	SYSTEM_publish_message(TIMER, INTERVAL_125US, NULL);
	for (int i = 1; i < 9; ++i)
		if ((cnt & ((1 << i) - 1)) == 0)
			SYSTEM_publish_message(TIMER, i, NULL);
	++cnt;
}

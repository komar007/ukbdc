#include "timer.h"
#include "aux.h"

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

#include "leds.h"
#include "main.h"
static struct timer_handler tmr_handlers[] = {
	{.callback = LED_timer_slow_handler, .cnt_max = 100},
	/* when in sleep mode, the main clock is divided by 8, so this is
	 * equivalent to 8000 */
	{.callback = MAIN_sleep_timer_handler, .cnt_max = 125}
};

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	for (unsigned i = 0; i < ARR_SZ(tmr_handlers); ++i) {
		++tmr_handlers[i].cnt;
		if (tmr_handlers[i].cnt > tmr_handlers[i].cnt_max) {
			tmr_handlers[i].cnt = 0;
			(*tmr_handlers[i].callback)();
		}
	}
}

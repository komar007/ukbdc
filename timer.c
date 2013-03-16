#include "timer.h"
#include "aux.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void TIMER_init()
{
	/* set up timer 0 to no prescale */
	TCCR0A = 0x00;
	TCCR0B = 0x01;
	TIMSK0 = _BV(TOIE0);
}

#include "leds.h"
#include "main.h"
static struct timer_fast_handler tmr_fhandlers[] = {
	{.callback = LED_timer_pwm_handler}
};
static struct timer_handler tmr_handlers[] = {
	{.callback = LED_timer_slow_handler, .cnt_max = 1000},
	{.callback = MAIN_sleep_timer_handler, .cnt_max = 10000}
};

ISR(TIMER0_OVF_vect)
{
	for (unsigned i = 0; i < ARR_SZ(tmr_fhandlers); ++i)
		(*tmr_fhandlers[i].callback)();
	for (unsigned i = 0; i < ARR_SZ(tmr_handlers); ++i) {
		++tmr_handlers[i].cnt;
		if (tmr_handlers[i].cnt > tmr_handlers[i].cnt_max) {
			tmr_handlers[i].cnt = 0;
			(*tmr_handlers[i].callback)();
		}
	}
}

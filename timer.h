#pragma once

#include <stdint.h>

/* A tick every 1/8th of a milisecond */
#define CYCLES_BETWEEN_TICKS 2048

struct timer_handler {
	void (*callback)();
	uint32_t cnt_max;
	uint32_t cnt;
};

void TIMER_init();

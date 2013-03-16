#pragma once

#include <stdint.h>

struct timer_handler {
	void (*callback)();
	uint32_t cnt_max;
	uint32_t cnt;
};

struct timer_fast_handler {
	void (*callback)();
};

void TIMER_init();

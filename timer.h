#pragma once

#include <stdint.h>

/* A tick every 1/8th of a milisecond */
#define CYCLES_BETWEEN_TICKS 2048

#define INTERVAL_125US	0
#define INTERVAL_250US	1
#define INTERVAL_500US	2
#define INTERVAL_1MS	3
#define INTERVAL_2MS	4
#define INTERVAL_4MS	5
#define INTERVAL_8MS	6
#define INTERVAL_16MS	7
#define INTERVAL_32MS	8

void TIMER_init();

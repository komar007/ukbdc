#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_TIMERS 8

#define ERR_TOO_MANY_TIMERS -1
#define ERR_NO_TIMER -2

/* parameters */
#define CONTINUOUS	(1 << 0)
#define DELETED		(1 << 1)

typedef struct {
	uint32_t on_tick;
	uint16_t period;
	uint8_t params;
	uint8_t id;
} timer_t;

void TIMER_init();
/* Adds a timer which will launch in time ticks. Continuous will make it
 * launch indefinitely every time ticks */
int8_t TIMER_add(uint32_t time, bool continuous);
/* Deletes one timer. Returns 0 on success or ERR_NO_TIMER */
int8_t TIMER_delete(int8_t id);

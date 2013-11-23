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

#include "timer.h"
#include "aux.h"
#include "system.h"

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t cycle = 0;
static volatile int ntmrs = 0;
static volatile timer_t heap[MAX_TIMERS];
static volatile bool heap_lock = false;
static volatile bool id_taken[MAX_TIMERS];

void TIMER_init()
{
	for (int i = 0; i < MAX_TIMERS; ++i)
		id_taken[i] = false;
	TCCR1A = 0x00;
	TCCR1B = 0x05; /* prescaling: 1024x */
	/* the timer will increment every 1/16 of a milisecond @ 16MHz */
	TIMSK1 = _BV(TOIE1);
}

static inline int8_t left(int8_t node)
{
	return node*2 + 1;
}
static inline int8_t right(int8_t node)
{
	return node*2 + 2;
}
static inline int8_t parent(int8_t node)
{
	return (node+1)/2 - 1;
}
static inline void swap(int8_t a, int8_t b)
{
	timer_t t = heap[a];
	heap[a] = heap[b];
	heap[b] = t;
}

static int8_t up_heap(int8_t n)
{
	while (parent(n) >= 0 && heap[parent(n)].on_tick > heap[n].on_tick) {
		swap(n, parent(n));
		n = parent(n);
	}
	return n;
}
static inline int8_t smallest(int8_t n)
{
	int8_t sm = n;
	if (left(n) < ntmrs && heap[left(n)].on_tick < heap[sm].on_tick)
		sm = left(n);
	if (right(n) < ntmrs && heap[right(n)].on_tick < heap[sm].on_tick)
		sm = right(n);
	return sm;
}
static void down_heap(int8_t n)
{
	int8_t s;
	for (; (s = smallest(n)) != n; n = s)
		swap(n, s);
}

static inline void schedule_next_ocr()
{
	OCR1A = heap[0].on_tick & 0x0000ffff;
	TIMSK1 |= _BV(OCIE1A);
}
static inline void unschedule_ocr()
{
	TIMSK1 &= ~_BV(OCIE1A);
}
/* returns if there are tasks to be run in this cycle */
static bool cycle_has_tasks()
{
	return ntmrs > 0 && cycle == (heap[0].on_tick >> 16) &&
		heap[0].on_tick >= ((uint32_t)cycle << 16) + TCNT1;
}

int8_t TIMER_add(uint32_t time, bool continuous)
{
	int id;
	for (id = 0; id < MAX_TIMERS && id_taken[id]; ++id)
		;
	if (id == MAX_TIMERS)
		return ERR_TOO_MANY_TIMERS;
	heap_lock = true;
	heap[ntmrs++] = (timer_t) {
		.on_tick = TCNT1 + ((uint32_t)cycle << 16) + time,
		.period = time,
		.params = continuous ? CONTINUOUS : 0,
		.id = id
	};
	id_taken[id] = true;
	bool min_changed = false;
	if (up_heap(ntmrs - 1) == 0)
		min_changed = true;
	if (min_changed && cycle_has_tasks())
		schedule_next_ocr();
	heap_lock = false;
	return id;
}

int8_t TIMER_delete(int8_t id)
{
	heap_lock = true;
	int i;
	for (i = 0; i < ntmrs && heap[i].id != id; ++i)
		;
	if (i >= ntmrs) {
		heap_lock = false;
		return ERR_NO_TIMER;
	}
	heap[i].params |= DELETED;
	heap[i].params &= ~CONTINUOUS;
	heap_lock = false;
	return 0;
}

ISR(TIMER1_OVF_vect)
{
	while (heap_lock)
		;
	++cycle;
	if (cycle_has_tasks())
		schedule_next_ocr();
}

ISR(TIMER1_COMPA_vect)
{
	while (heap_lock)
		;
	while (ntmrs > 0 && heap[0].on_tick <= ((uint32_t)cycle << 16) + TCNT1) {
		swap(0, --ntmrs);
		down_heap(0);
		timer_t deleted = heap[ntmrs];
		/* this call may potentially alter the heap (handler can
		 * modify timer heap to reschedule) */
		if (!(deleted.params & DELETED))
			SYSTEM_publish_message(TIMER, deleted.id, (void*)&deleted.id);
		/* if timer was continuous, re-add */
		if (deleted.params & CONTINUOUS) {
			deleted.on_tick += deleted.period;
			heap[ntmrs++] = deleted;
			up_heap(ntmrs - 1);
		} else {
			id_taken[deleted.id] = false;
		}
	}
	if (cycle_has_tasks())
		schedule_next_ocr();
	else
		unschedule_ocr();
}

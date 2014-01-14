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

/*! \defgroup TIMER
 * \brief Hardware-supported single-shot and continuous timers
 *
 * This module allows the user to add and delete timers which will trigger
 * events after a particular time.
 * Each timer can be continuous, which means it will trigger continuously
 * every amount of time, or not continuous which means it will trigger only
 * once after a particular time period.
 *  @{
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/*! The maximum number of timers which can be used in the system at the same
 * time */
#define MAX_TIMERS 8

/*! Error occuring when trying to add too many timers */
#define ERR_TOO_MANY_TIMERS -1
/*! Error occuring when trying to delete a non-existent timer */
#define ERR_NO_TIMER -2

/* parameters */
#define CONTINUOUS	(1 << 0)
#define DELETED		(1 << 1)

/*! Structure used to store a single timer on heap */
typedef struct {
	/*! The time when the timer is triggered */
	uint32_t on_tick;
	/*! The time delay from creation to the time the timer is triggered or
	 * delay between triggers (for continuous timers) */
	uint16_t period;
	/*! Configuration flags */
	uint8_t params;
	/*! Timer's identification number */
	uint8_t id;
} timer_t;

/*! Initializes the TIMER module. This function must be called before any other
 * function in this module */
void TIMER_init();
/*! Adds a timer which will launch in `time` ticks.
 * \param time the time (in ticks) after which the timer will trigger
 * \param continuous make the timer launch indefinitely every `time` ticks
 * \return id of created timer
 * \retval ERR_TOO_MANY_TIMERS the timer did not fit on the heap
 */
int8_t TIMER_add(uint32_t time, bool continuous);
/*! Deletes one timer.
 * \retval 0 successfully deleted
 * \retval ERR_NO_TIMER timer does not exist
 */
int8_t TIMER_delete(int8_t id);

/*! @} */

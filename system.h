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

#pragma once

#include <stdint.h>
#include "list.h"

#define ERR_WRONG_TYPE -1
#define ANY 0xff

typedef void (*task_callback_t)();

typedef struct {
	int priority;
	int last_run;
	task_callback_t callback;
} task_t;

#include "system_messages.h"

typedef void (*message_callback_t)(void *data);

typedef struct {
	uint8_t subtype;
	message_callback_t callback;
} message_handler_t;

void SYSTEM_init();
void SYSTEM_add_task(task_callback_t callback, int priority);
void SYSTEM_main_loop();

/* Notify all the subscribers of message's type and/or subtype */
int SYSTEM_publish_message(message_type_t type, uint8_t subtype, void *data);
/* Subscribe to a particular message type. The subscriber will be called only
 * on a message with specified subtype */
int SYSTEM_subscribe(message_type_t type, uint8_t subtype, message_callback_t handler);

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

#include "system.h"
#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include <avr/interrupt.h>

list_t task_list;
static list_t handlers_for_type[NUM_SYSTEM_MESSAGE_TYPES];

void SYSTEM_init()
{
	list_init(&task_list);
	for (int i = 0; i < NUM_SYSTEM_MESSAGE_TYPES; ++i)
		list_init(&handlers_for_type[i]);
	sei();
}

void SYSTEM_add_task(task_callback_t callback, int priority)
{
	task_t *new_task = calloc(1, sizeof(*new_task));
	*new_task = (task_t){
		.priority = priority,
		.callback = callback
	};
	list_add(&task_list, new_task);
}

void SYSTEM_main_loop()
{
	while (true) {
		node_t *i = task_list.head;
		for (; i ; i = i->next) {
			task_t *task = i->data;
			task->last_run = -1 - task->priority;
		}
		for (int cycle = 0; cycle < INT_MAX; ++cycle) {
			node_t *i = task_list.head;
			for (; i ; i = i->next) {
				task_t *task = i->data;
				if (task->last_run + task->priority < cycle)
					task->callback(), task->last_run = cycle;
			}
		}
	}
}

int SYSTEM_publish_message(message_type_t type, uint8_t subtype, void *data)
{
	if (type >= NUM_SYSTEM_MESSAGE_TYPES)
		return ERR_WRONG_TYPE;
	node_t *i = handlers_for_type[type].head;
	for (; i; i = i->next) {
		message_handler_t *handler = i->data;
		if (handler->subtype == ANY || handler->subtype == subtype)
			handler->callback(data);
	}
	return 0;
}

int SYSTEM_subscribe(message_type_t type, uint8_t subtype, message_callback_t callback)
{
	if (type >= NUM_SYSTEM_MESSAGE_TYPES)
		return ERR_WRONG_TYPE;
	message_handler_t *handler = calloc(1, sizeof(*handler));
	*handler = (message_handler_t){
		.subtype = subtype,
		.callback = callback
	};
	list_t *handlers = &handlers_for_type[type];
	list_add(handlers, handler);
	return 0;
}

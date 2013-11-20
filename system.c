#include "system.h"
#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

list_t task_list;
static list_t handlers_for_type[NUM_SYSTEM_MESSAGE_TYPES];

void SYSTEM_init()
{
	list_init(&task_list);
	for (int i = 0; i < NUM_SYSTEM_MESSAGE_TYPES; ++i)
		list_init(&handlers_for_type[i]);
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

int SYSTEM_publish_message(message_type_t type, void *data)
{
	if (type >= NUM_SYSTEM_MESSAGE_TYPES)
		return ERR_WRONG_TYPE;
	node_t *i = handlers_for_type[type].head;
	for (; i; i = i->next) {
		message_handler_t handler = i->data;
		handler(type, data);
	}
	return 0;
}

int SYSTEM_subscribe(message_type_t type, message_handler_t handler)
{
	if (type >= NUM_SYSTEM_MESSAGE_TYPES)
		return ERR_WRONG_TYPE;
	list_t *handlers = &handlers_for_type[type];
	list_add(handlers, handler);
	return 0;
}

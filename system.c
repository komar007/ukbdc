#include "system.h"
#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

list_t task_list;

void SYSTEM_init()
{
	list_init(&task_list);
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

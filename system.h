#pragma once

#include <stdint.h>
#include "list.h"

#define ERR_WRONG_TYPE -1

typedef void (*task_callback_t)();

typedef struct {
	int priority;
	int last_run;
	task_callback_t callback;
} task_t;

#include "system_messages.h"

typedef void (*message_handler_t)(message_type_t type, void *data);

void SYSTEM_init();
void SYSTEM_add_task(task_callback_t callback, int priority);
void SYSTEM_main_loop();

int SYSTEM_publish_message(message_type_t type, void *data);
int SYSTEM_subscribe(message_type_t type, message_handler_t handler);

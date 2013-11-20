#include <stdint.h>

typedef void (*task_callback_t)();

typedef struct {
	int priority;
	int last_run;
	task_callback_t callback;
} task_t;

void SYSTEM_init();
void SYSTEM_add_task(task_callback_t callback, int priority);
void SYSTEM_main_loop();

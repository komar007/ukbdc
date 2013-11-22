#pragma once

#include "system.h"

#define LAYOUT_BEGIN 0x4000

void MAIN_handle_sof(void *data);
void MAIN_sleep_timer_handler();

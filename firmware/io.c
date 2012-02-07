#include "io.h"
#include "io_impl.h"

#include <stdbool.h>

bool IO_get(uint8_t pin)
{
	if (pin & 0x80)
		return IO_get_external(pin);
	else
		return IO_get_internal(pin);
}

void IO_set(uint8_t pin, bool val)
{
	if (pin & 0x80)
		IO_set_external(pin, val);
	else
		IO_set_internal(pin, val);
}

void IO_config(uint8_t pin, bool dir)
{
	if (pin & 0x80)
		IO_config_external(pin, dir);
	else
		IO_config_internal(pin, dir);
}

#include "atmel_bootloader.h"
#include <avr/interrupt.h>

void flash_write_page(uint32_t addr, const uint8_t *data)
{
	volatile uint8_t sreg = SREG;
	cli();
	for (int i = 0; i < SPM_PAGESIZE; i += 2)
		flash_fill_temp_buffer(*(uint16_t*)(data + i), i);
	flash_page_erase_and_write(addr);
	SREG = sreg;
}

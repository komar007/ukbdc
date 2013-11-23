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

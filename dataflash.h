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

/* [Commands] -------------------------------------------------------------- */
#define CONTINUOUS_ARRAY_READ_LOW	0x03
#define PROGRAM_THROUGH_BUFFER		0x82
#define STATUS_REGISTER_READ		0xD7
/* [/Commands] ------------------------------------------------------------- */

void DATAFLASH_write_page(uint16_t page_addr, uint16_t size, const uint8_t *buffer);
void DATAFLASH_read_page(uint16_t page_addr, uint16_t size, uint8_t *buffer);
uint8_t DATAFLASH_status();

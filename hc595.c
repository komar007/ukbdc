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

#include "platforms.h"

void HC595_send_byte(uint8_t byte)
{
	for (uint8_t mask = (1 << 7); mask != 0; mask >>= 1) {
		if (byte & mask)
			HC595_SER_HIGH();
		else
			HC595_SER_LOW();
		/* send bit */
		HC595_CLK_HIGH();
		HC595_CLK_LOW();
	}
}

void HC595_set_outputs(uint16_t word)
{
	HC595_BUFEN_SET();
	HC595_send_byte(word & 0x00ff);
	HC595_send_byte((uint8_t)(word >> 8));
	HC595_LATCH_HIGH();
	HC595_LATCH_LOW();
}

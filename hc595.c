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

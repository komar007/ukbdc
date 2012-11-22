#include "platforms.h"
#include "dataflash.h"

#include <util/delay.h>

void DATAFLASH_send_byte(uint8_t byte)
{
	for (uint8_t mask = (1 << 7); mask != 0; mask >>= 1) {
		if (byte & mask)
			DATAFLASH_SI_HIGH();
		else
			DATAFLASH_SI_LOW();
		/* send bit */
		DATAFLASH_CLK_HIGH();
		DATAFLASH_CLK_LOW();
	}
}

uint8_t DATAFLASH_recv_byte()
{
	uint8_t byte = 0;
	for (uint8_t i = 0; i < 8; ++i) {
		byte <<= 1;
		byte |= DATAFLASH_SO_STATE();
		/* get next bit */
		DATAFLASH_CLK_HIGH();
		DATAFLASH_CLK_LOW();
	}
	return byte;
}

uint8_t DATAFLASH_status()
{
	DATAFLASH_CS_SET();
	DATAFLASH_send_byte(STATUS_REGISTER_READ);
	uint8_t status = DATAFLASH_recv_byte();
	DATAFLASH_CS_CLEAR();
	return status;
}

void DATAFLASH_write_page(uint16_t page_addr, uint16_t size, const uint8_t *buffer)
{
	DATAFLASH_CS_SET();
	DATAFLASH_send_byte(PROGRAM_THROUGH_BUFFER);
	DATAFLASH_send_byte((uint8_t)(page_addr >> 6));
	DATAFLASH_send_byte((uint8_t)(page_addr << 2));
	DATAFLASH_send_byte(0x00);
	for (uint16_t i = 0; i < size; ++i)
		DATAFLASH_send_byte(buffer[i]);
	DATAFLASH_CS_CLEAR();
}

void DATAFLASH_read_page(uint16_t page_addr, uint16_t size, uint8_t *buffer)
{
	DATAFLASH_CS_SET();
	DATAFLASH_send_byte(CONTINUOUS_ARRAY_READ_LOW);
	DATAFLASH_send_byte((uint8_t)(page_addr >> 6));
	DATAFLASH_send_byte((uint8_t)(page_addr << 2));
	DATAFLASH_send_byte(0x00);
	for (uint16_t i = 0; i < size; ++i)
		buffer[i] = DATAFLASH_recv_byte();
	DATAFLASH_CS_CLEAR();
}

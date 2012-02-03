#pragma once

#include <stdint.h>

/* [Commands] -------------------------------------------------------------- */
#define CONTINUOUS_ARRAY_READ_LOW	0x03
#define PROGRAM_THROUGH_BUFFER		0x82
#define STATUS_REGISTER_READ		0xD7
/* [/Commands] ------------------------------------------------------------- */

void DATAFLASH_write_page(uint16_t page_addr, const uint8_t *buffer);
void DATAFLASH_read_page(uint16_t page_addr, uint8_t *buffer);
uint8_t DATAFLASH_status();

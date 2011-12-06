#include <avr/pgmspace.h>

#include "usb_hardware.h"
#include "usb_config.h"
#include "usb.h"

void USB_OUT_read_buffer(void *ptr, uint8_t len)
{
	while (len-- > 0)
		*((uint8_t*)ptr++) = USB_OUT_read_byte();
}

void USB_IN_write_buffer(const void *ptr, uint8_t len)
{
	while (len-- > 0)
		USB_IN_write_byte(*(uint8_t*)ptr++);
}
void USB_IN_write_buffer_P(const uint8_t *ptr, uint8_t len)
{
	while (len-- > 0)
		USB_IN_write_byte(pgm_read_byte(ptr++));
}

bool USB_write_blob(const void *ptr, uint16_t len,
		uint16_t ep_size, bool progmem)
{
	while (len > 0) {
		/* wait for readiness or for abort OUT packet */
		while (bit_is_clear(UEINTX, TXINI) &&
				bit_is_clear(UEINTX, RXOUTI))
			;
		if (bit_is_set(UEINTX, RXOUTI)) {
			/* received OUT ZLP (abort) */
			USB_ack_OUT();
			return false;
		}
		uint16_t packet_len = len <= ep_size ? len : ep_size;
		if (progmem)
			USB_IN_write_buffer_P(ptr, packet_len);
		else
			USB_IN_write_buffer(ptr, packet_len);
		USB_flush_IN();
		ptr = (uint8_t*)ptr + packet_len;
		len -= packet_len;
	}
	return true;
}

bool USB_configure_endpoint(uint8_t num)
{
	uint8_t i = 0;
	for (; i < NUM_ENDPOINTS; ++i) {
		uint8_t cur_num = pgm_read_byte(&endpoint_configs[i]);
		if (cur_num == num)
			break;
	}
	if (i >= NUM_ENDPOINTS)
		return false;
	struct endpoint_config config;
	*(uint32_t*)&config = pgm_read_dword(&endpoint_configs[i]);
	UENUM   = config.num;
	UECONX  = _BV(EPEN);
	UECFG0X = config.type;
	UECFG1X = config.config;
	/* set interrupt enable bits */
	UEIENX  = config.int_flags;
	return true;
}

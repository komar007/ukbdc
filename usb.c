/* USB Keyboard Example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2009 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "aux.h"
#include "usb.h"
#include "usb_config.h"

#include <stdint.h>
#include <avr/interrupt.h>

#include "descriptors.h"


/* current USB configuration chosen by host (0 means no config chosen yet) */
static volatile uint8_t usb_current_conf = 0;

/* [Public API section] ---------------------------------------------------- */

/* initialize USB */
void USB_init()
{
	USB_set_pads_regulator(true);
	USB_pll_config(CRYSTAL_16MHZ);
	/* wait for PLL to lock to the clock */
        while (!PLL_is_locked())
		;
        USB_enable();
	/* make sure DETACH is off, and low speed mode is not set */
	UDCON &= ~(_BV(DETACH) | _BV(LSM));
	usb_current_conf = 0;
        UDIEN = _BV(EORSTE) | _BV(SOFE);
	sei();
}

/* return 0 if the USB is not configured, or the configuration number
 * selected by the HOST */
uint8_t USB_get_configuration()
{
	return usb_current_conf;
}

/* [/Public API section] --------------------------------------------------- */

/* [Interrupt handlers section] -------------------------------------------- */

/* USB Device Interrupt
 * This currently handles USB End Of Reset and Start Of Frame */
ISR(USB_GEN_vect)
{
        uint8_t device_int_flags = UDINT;
	/* clear all device interrupt flags */
        UDINT = 0x00;
        if (device_int_flags & _BV(EORSTI)) {
		/* on end of reset configure endpoint 0 */
		USB_configure_endpoint(0,
				EP_TYPE_CONTROL,
				EP_SIZE_32 | EP_SINGLE_BUFFER,
				_BV(RXSTPE));
		usb_current_conf = 0;
        }
	if (device_int_flags & _BV(SOFI) && usb_current_conf) {
		/* call all SOF handlers */
		for (uint8_t i = 0; i < ARR_SZ(sof_handlers); ++i)
			(*sof_handlers[i].function)();
	}
}


static void serve_get_descriptor(uint16_t wValue, uint16_t wIndex, uint16_t wLength)
{
	const uint8_t *list;
	list = (const uint8_t *)descriptor_list;
	uint8_t	desc_length;
	const uint8_t *desc_addr;
	for (uint8_t i=0; ; ++i) {
		if (i >= NUM_DESC_LIST) {
			USB_stall_endpoint();
			return;
		}
		uint16_t desc_val = pgm_read_word(list);
		if (desc_val != wValue) {
			list += sizeof(struct descriptor_list_struct);
			continue;
		}
		list += 2;
		desc_val = pgm_read_word(list);
		if (desc_val != wIndex) {
			list += sizeof(struct descriptor_list_struct)-2;
			continue;
		}
		list += 2;
		desc_addr = (const uint8_t *)pgm_read_word(list);
		list += 2;
		desc_length = pgm_read_byte(list);
		break;
	}
	uint8_t len = (wLength < 256) ? wLength : 255;
	bool status = USB_write_blob(desc_addr, len, ENDPOINT0_SIZE, true);
	if (status)
		USB_control_read_complete_status_stage();
}

/* Processes Standard Device Requests. Returns true on no error, false if the
 * request couldn't be processed or is not supported */
/* static inline just for size optimization */
static inline bool process_standard_device_requests(struct setup_packet *s)
{
	if (request(s, SET_ADDRESS)) {
		USB_set_addr((uint8_t)s->wValue);
		USB_control_write_complete_status_stage();
		/* make sure the status stage is over before enabling
		 * the new address */
		USB_wait_IN();
		USB_addr_enable();
	} else if (request(s, SET_CONFIGURATION)) {
		usb_current_conf = s->wValue;
		USB_control_write_complete_status_stage();
		USB_configure_endpoint(KEYBOARD_ENDPOINT,
				EP_TYPE_INTERRUPT_IN,
				EP_SIZE_32 | EP_DOUBLE_BUFFER,
				0x00);
		USB_reset_endpoint_fifo(KEYBOARD_ENDPOINT);
	} else if (request(s, GET_CONFIGURATION)) {
		USB_wait_IN();
		USB_IN_write_byte(usb_current_conf);
		USB_flush_IN();
		USB_control_read_complete_status_stage();
	} else if (request(s, GET_STATUS)) {
		USB_wait_IN();
		/* bus powered, no remote wakeup */
		USB_IN_write_word(0x0000);
		USB_flush_IN();
		USB_control_read_complete_status_stage();
	} else if (request(s, GET_DESCRIPTOR)) {
		serve_get_descriptor(s->wValue, s->wIndex, s->wLength);
	} else {
		return false;
	}
	return true;
}

/* Processes Standard Endpoint Requests. Returns true on no error, false if the
 * request couldn't be processed or is not supported */
/* static inline just for size optimization */
static inline bool process_standard_endpoint_requests(struct setup_packet *s)
{
	if (s->bRequest == GET_STATUS) {
		USB_wait_IN();
		int status;
		USB_set_endpoint(s->wIndex);
		if (USB_endpoint_stalled())
			status = 1;
		else
			status = 0;
		USB_set_endpoint(0);
		USB_IN_write_word(status);
		USB_flush_IN();
		USB_control_read_complete_status_stage();
	} else if ((s->bRequest == CLEAR_FEATURE || s->bRequest == SET_FEATURE)
			&& s->wValue == ENDPOINT_HALT) {
		int i = s->wIndex & 0x7F;
		if (i >= 1 && i <= MAX_ENDPOINT) {
			USB_control_write_complete_status_stage();
			USB_set_endpoint(i);
			if (s->bRequest == SET_FEATURE) {
				USB_stall_endpoint();
			} else {
				USB_unstall_endpoint();
				USB_reset_endpoint_fifo(i);
			}
			USB_set_endpoint(0);
		} else {
			return false;
		}
	} else {
		return false;
	}
	return true;
}

/* Processes Standard Interface Requests. Returns true on no error, false if the
 * request couldn't be processed or is not supported */
/* static inline just for size optimization */
static inline bool process_standard_interface_requests(struct setup_packet *s)
{
	if (request(s, GET_DESCRIPTOR)) {
		serve_get_descriptor(s->wValue, s->wIndex, s->wLength);
	} else {
		return false;
	}
	return true;
}

/* This handles only start-of-frame so far */
ISR(USB_COM_vect)
{
        USB_set_endpoint(0);
        if (bit_is_set(UEINTX, RXSTPI)) {
		bool all_ok = false;
		struct setup_packet s;
		USB_OUT_read_buffer(&s, 8);
		/* acknowledge setup *after* reading, because it clears the bank */
		USB_ack_SETUP();
		/* process all Standard Device Requests */
		if        (request_type(&s, TYPE | RECIPIENT, STANDARD | DEVICE)) {
			all_ok = process_standard_device_requests(&s);
		} else if (request_type(&s, TYPE | RECIPIENT, STANDARD | INTERFACE)) {
			all_ok = process_standard_interface_requests(&s);
		} else if (request_type(&s, TYPE | RECIPIENT, STANDARD | ENDPOINT)) {
			all_ok = process_standard_endpoint_requests(&s);
		} else if (request_type(&s, TYPE | RECIPIENT, CLASS    | INTERFACE)) {
			bool found = false;
			for (uint8_t i = 0; i < ARR_SZ(iface_req_handlers); ++i) {
				if (iface_req_handlers[i].iface_number == s.wIndex) {
					found = (*iface_req_handlers[i].function)(&s);
					break;
				}
			}
			if (found) {
				if (request_type(&s, DIRECTION, DEVICE_TO_HOST))
					USB_control_read_complete_status_stage();
				else
					USB_control_write_complete_status_stage();
				all_ok = true;
			}
		}
		if (!all_ok)
			USB_stall_endpoint();
	}
}

/* [/Interrupt handlers section] ------------------------------------------- */

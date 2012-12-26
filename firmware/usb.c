#include "aux.h"
#include "usb.h"
#include "usb_config.h"
#include "descriptors.h"

#include <stdint.h>
#include <avr/interrupt.h>

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

/* deinitialize USB */
void USB_close()
{
        UDIEN &= ~(_BV(EORSTE) | _BV(SOFE));
	usb_current_conf = 0;
	UDCON |= _BV(DETACH) | _BV(LSM);
        USB_disable();
	USB_set_pads_regulator(false);
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
	uint8_t prev_endp = USB_get_endpoint();
        uint8_t device_int_flags = UDINT;
	/* clear all device interrupt flags */
        UDINT = 0x00;
        if (device_int_flags & _BV(EORSTI)) {
		/* on end of reset configure endpoint 0 */
		USB_configure_endpoint(0);
		usb_current_conf = 0;
		goto end;
        }
	if (device_int_flags & _BV(SOFI) && usb_current_conf) {
		/* call all SOF handlers */
		for (uint8_t i = 0; i < NUM_SOF_HANDLERS; ++i)
			(*sof_handlers[i].f)();
	}
end:
	USB_set_endpoint(prev_endp);
}

/* FIXME: This code comes from the Teensy hid keyboard example and is
 * absolutely horrible and needs a complete rewrite */
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
	/* enable interrupts to handle reset in the middle of sending a
	 * descriptor during enumeration */
	uint8_t sreg = SREG;
	sei();
	bool status = USB_write_blob(desc_addr, wLength, ENDPOINT0_SIZE, true);
	if (status)
		USB_control_read_complete_status_stage();
	SREG = sreg;
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
		for (uint8_t i = 1; i < NUM_ENDPOINTS; ++i) {
			if (!USB_configure_endpoint(i))
				return false;
			USB_reset_endpoint_fifo(KEYBOARD_ENDPOINT);
		}
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
	if (request(s, GET_STATUS)) {
		USB_wait_IN();
		USB_set_endpoint(s->wIndex);
		uint8_t status = USB_endpoint_stalled() ? 0x01 : 0x00;
		USB_set_endpoint(0);
		USB_IN_write_word((uint16_t)status);
		USB_flush_IN();
		USB_control_read_complete_status_stage();
	} else if ((request(s, CLEAR_FEATURE) || request(s, SET_FEATURE))
			&& s->wValue == ENDPOINT_HALT) {
		uint16_t i = s->wIndex & 0x7F;
		if (i >= 1 && i < NUM_ENDPOINTS) {
			USB_control_write_complete_status_stage();
			USB_set_endpoint(i);
			if (request(s, SET_FEATURE)) {
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

/* Processes Class Interface Requests. Returns true on no error, false if the
 * request couldn't be processed or is not supported. Actual processing is
 * performed using interface request handlers */
/* static inline just for size optimization */
static inline bool process_class_interface_requests(struct setup_packet *s)
{
	bool found = false;
	for (uint8_t i = 0; i < NUM_INTERFACE_REQUEST_HANDLERS; ++i) {
		if (get_pgm_struct_field(&iface_req_handlers[i], iface_num) ==
				(s->wIndex & 0xFF)) {
			interface_request_handler_fun handler = (void*)(uint16_t)
				get_pgm_struct_field(&iface_req_handlers[i], f);
			found = (*handler)(s);
			break;
		}
	}
	if (!found)
		return false;
	if (request_type(s, DIRECTION, DEVICE_TO_HOST))
		USB_control_read_complete_status_stage();
	else
		USB_control_write_complete_status_stage();
	return true;
}

static inline void handle_setup_packet()
{
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
		all_ok = process_class_interface_requests(&s);
	}
	if (!all_ok)
		USB_stall_endpoint();
}

/* Handle USB events */
#define ENDPOINT_EVENTS (_BV(RXOUTI) | _BV(TXINI) | _BV(STALLEDI) | _BV(NAKOUTI) | _BV(NAKINI))
ISR(USB_COM_vect)
{
	uint8_t prev_endp = USB_get_endpoint();
        USB_set_endpoint(0);
        if (bit_is_set(UEINTX, RXSTPI)) {
		handle_setup_packet();
		goto end;
	}
	for (int i = 0; i < NUM_ENDPOINT_INTERRUPT_HANDLERS; ++i) {
		uint8_t endpoint_number = get_pgm_struct_field(
				&endpoint_int_handlers[i], endpoint_num);
		USB_set_endpoint(endpoint_number);
		if (UEINTX & ENDPOINT_EVENTS) {
			void (*fun)(uint8_t flags) = (void*)(uint16_t)
				get_pgm_struct_field(&endpoint_int_handlers[i], f);
			(*fun)(UEINTX);
		}
	}
end:
	USB_set_endpoint(prev_endp);
}

/* [/Interrupt handlers section] ------------------------------------------- */

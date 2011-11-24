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

// Version 1.0: Initial Release
// Version 1.1: Add support for Teensy 2.0

#define USB_SERIAL_PRIVATE_INCLUDE
#include "usb_keyboard.h"
#include "usb.h"

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/

// You can change these to give your code its own name.
#define STR_MANUFACTURER	L"komar"
#define STR_PRODUCT		L"komar board 0.1"


// Mac OS-X and Linux automatically load the correct drivers.  On
// Windows, even though the driver is supplied by Microsoft, an
// INF file is needed to load the driver.  These numbers need to
// match the INF file.
#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x047C


// USB devices are supposed to implment a halt feature, which is
// rarely (if ever) used.  If you comment this line out, the halt
// code will be removed, saving 102 bytes of space (gcc 4.3.0).
// This is not strictly USB compliant, but works with all major
// operating systems.
#define SUPPORT_ENDPOINT_HALT



/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE		32

#define KEYBOARD_INTERFACE	0
#define KEYBOARD_ENDPOINT	3
#define KEYBOARD_SIZE		32

/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!


static uint8_t PROGMEM device_descriptor[] = {
	18,					// bLength
	1,					// bDescriptorType
	0x00, 0x02,				// bcdUSB
	0,					// bDeviceClass
	0,					// bDeviceSubClass
	0,					// bDeviceProtocol
	ENDPOINT0_SIZE,				// bMaxPacketSize0
	LSB(VENDOR_ID), MSB(VENDOR_ID),		// idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),	// idProduct
	0x00, 0x01,				// bcdDevice
	1,					// iManufacturer
	2,					// iProduct
	0,					// iSerialNumber
	1					// bNumConfigurations
};

// Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
static uint8_t PROGMEM keyboard_hid_report_desc[] = {
        0x05, 0x01,          // Usage Page (Generic Desktop),
        0x09, 0x06,          // Usage (Keyboard),
        0xA1, 0x01,          // Collection (Application),
        0x75, 0x01,          //   Report Size (1),
        0x95, 0x08,          //   Report Count (8),
        0x05, 0x07,          //   Usage Page (Key Codes),
        0x19, 0xE0,          //   Usage Minimum (224),
        0x29, 0xE7,          //   Usage Maximum (231),
        0x15, 0x00,          //   Logical Minimum (0),
        0x25, 0x01,          //   Logical Maximum (1),
        0x81, 0x02,          //   Input (Data, Variable, Absolute), ;Modifier byte
        0x95, 0x01,          //   Report Count (1),
        0x75, 0x08,          //   Report Size (8),
        0x81, 0x03,          //   Input (Constant),                 ;Reserved byte
        0x95, 0x05,          //   Report Count (5),
        0x75, 0x01,          //   Report Size (1),
        0x05, 0x08,          //   Usage Page (LEDs),
        0x19, 0x01,          //   Usage Minimum (1),
        0x29, 0x05,          //   Usage Maximum (5),
        0x91, 0x02,          //   Output (Data, Variable, Absolute), ;LED report
        0x95, 0x01,          //   Report Count (1),
        0x75, 0x03,          //   Report Size (3),
        0x91, 0x03,          //   Output (Constant),                 ;LED report padding
        0x95, 30,          //   Report Count (7),
        0x75, 0x08,          //   Report Size (8),
        0x15, 0x00,          //   Logical Minimum (0),
        0x25, 0x68,          //   Logical Maximum(104),
        0x05, 0x07,          //   Usage Page (Key Codes),
        0x19, 0x00,          //   Usage Minimum (0),
        0x29, 0x68,          //   Usage Maximum (104),
        0x81, 0x00,          //   Input (Data, Array),
        0xc0                 // End Collection
};

#define CONFIG1_DESC_SIZE        (9+9+9+7)
#define KEYBOARD_HID_DESC_OFFSET (9+9)
static uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	// configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
	9, 					// bLength;
	2,					// bDescriptorType;
	LSB(CONFIG1_DESC_SIZE),			// wTotalLength
	MSB(CONFIG1_DESC_SIZE),
	1,					// bNumInterfaces
	1,					// bConfigurationValue
	0,					// iConfiguration
	0xC0,					// bmAttributes
	50,					// bMaxPower
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	KEYBOARD_INTERFACE,			// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,					// bInterfaceClass (0x03 = HID)
	0x00,					// bInterfaceSubClass (0x01 = Boot)
	0x00,					// bInterfaceProtocol (0x01 = Keyboard)
	0,					// iInterface
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,					// bDescriptorType
	0x11, 0x01,				// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,					// bDescriptorType
	sizeof(keyboard_hid_report_desc),	// wDescriptorLength
	0,
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	KEYBOARD_ENDPOINT | 0x80,		// bEndpointAddress
	0x03,					// bmAttributes (0x03=intr)
	KEYBOARD_SIZE, 0,			// wMaxPacketSize
	1					// bInterval
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};
static struct usb_string_descriptor_struct PROGMEM string0 = {
	4,
	3,
	{0x0409}
};
static struct usb_string_descriptor_struct PROGMEM string1 = {
	sizeof(STR_MANUFACTURER),
	3,
	STR_MANUFACTURER
};
static struct usb_string_descriptor_struct PROGMEM string2 = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static struct descriptor_list_struct {
	uint16_t	wValue;
	uint16_t	wIndex;
	const uint8_t	*addr;
	uint8_t		length;
} PROGMEM descriptor_list[] = {
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
	{0x2200, KEYBOARD_INTERFACE, keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
	{0x2100, KEYBOARD_INTERFACE, config1_descriptor+KEYBOARD_HID_DESC_OFFSET, 9},
	{0x0300, 0x0000, (const uint8_t *)&string0, 4},
	{0x0301, 0x0409, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
	{0x0302, 0x0409, (const uint8_t *)&string2, sizeof(STR_PRODUCT)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))


/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/

// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration=0;

// which modifier keys are currently pressed
// 1=left ctrl,    2=left shift,   4=left alt,    8=left gui
// 16=right ctrl, 32=right shift, 64=right alt, 128=right gui
uint8_t keyboard_modifier_keys=0;

// which keys are currently pressed, up to 6 keys may be down at once
uint8_t keyboard_keys[30]={0,0,0,0,0,0};

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t keyboard_protocol=0;

// the idle configuration, how often we send the report to the
// host (ms * 4) even when it hasn't changed
static uint8_t keyboard_idle_config=125;

// count until idle timeout
static uint8_t keyboard_idle_count=0;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
volatile uint8_t keyboard_leds=0;


/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/


// initialize USB
void usb_init(void)
{
	USB_set_pads_regulator(true);
	USB_pll_config(CRYSTAL_16MHZ);
	/* wait for PLL to lock to the clock */
        while (!PLL_is_locked())
		;
        USB_enable();
	/* make sure DETACH is off, and low speed mode is not set */
	UDCON &= ~(_BV(DETACH) | _BV(LSM));
	usb_configuration = 0;
        UDIEN = _BV(EORSTE)|_BV(SOFE);
	sei();
}

// return 0 if the USB is not configured, or the configuration
// number selected by the HOST
uint8_t usb_configured(void)
{
	return usb_configuration;
}



// send the contents of keyboard_keys and keyboard_modifier_keys
int8_t usb_keyboard_send(void)
{
	uint8_t old_sreg;

	if (!usb_configuration)
		return -1;
	old_sreg = SREG;
	cli();
	UENUM = KEYBOARD_ENDPOINT;
	uint8_t timeout = UDFNUML + 50;
	while (1) {
		// are we ready to transmit?
		if (bit_is_set(UEINTX, RWAL))
			break;
		SREG = old_sreg;
		// has the USB gone offline?
		if (!usb_configuration) return -1;
		// have we waited too long?
		if (UDFNUML == timeout) return -1;
		// get ready to try checking again
		old_sreg = SREG;
		cli();
		UENUM = KEYBOARD_ENDPOINT;
	}
	USB_IN_write_byte(keyboard_modifier_keys);
	USB_IN_write_byte(0);
	for (int i = 0; i < 30; i++)
		USB_IN_write_byte(keyboard_keys[i]);
	UEINTX &= ~(_BV(FIFOCON) | _BV(TXINI));
	keyboard_idle_count = 0;
	SREG = old_sreg;
	return 0;
}

/**************************************************************************
 *
 *  Private Functions - not intended for general user consumption....
 *
 **************************************************************************/



// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
	uint8_t  i;
	static uint8_t div4=0;

        uint8_t device_int_flags = UDINT;
	/* clear all device interrupt flags */
        UDINT = 0x00;
        if (device_int_flags & _BV(EORSTI)) {
		USB_configure_endpoint(0,
				EP_TYPE_CONTROL,
				EP_SIZE_32 | EP_SINGLE_BUFFER,
				_BV(RXSTPE));
		usb_configuration = 0;
        }
	if ((device_int_flags & _BV(SOFI)) && usb_configuration) {
		if (keyboard_idle_config != 0 && (++div4 & 3) == 0) {
			UENUM = KEYBOARD_ENDPOINT;
			if (bit_is_set(UEINTX, RWAL)) {
				keyboard_idle_count++;
				if (keyboard_idle_count == keyboard_idle_config) {
					keyboard_idle_count = 0;
					USB_IN_write_byte(keyboard_modifier_keys);
					USB_IN_write_byte(0);
					for (i=0; i<30; i++)
						USB_IN_write_byte(keyboard_keys[i]);
					UEINTX &= ~(_BV(FIFOCON) | _BV(TXINI));
				}
			}
		}
	}
}



static inline void usb_ack_out(void)
{
	UEINTX = ~_BV(RXOUTI);
}


void serve_get_descriptor(uint16_t wValue, uint16_t wIndex, uint16_t wLength)
{
	const uint8_t *list;
	list = (const uint8_t *)descriptor_list;
	uint8_t	desc_length;
	const uint8_t *desc_addr;
	for (uint8_t i=0; ; ++i) {
		if (i >= NUM_DESC_LIST) {
			UECONX = _BV(STALLRQ)|_BV(EPEN);  //stall
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
	uint8_t n = 0;
	if (len > desc_length) len = desc_length;
	do {
		// wait for host ready for IN packet
		while (bit_is_clear(UEINTX, TXINI) &&
				bit_is_clear(UEINTX, RXOUTI))
			;
		if (bit_is_set(UEINTX, RXOUTI))
			return;	// abort
		// send IN packet
		n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
		for (int i = n; i; --i)
			USB_IN_write_byte(pgm_read_byte(desc_addr++));
		len -= n;
		USB_ack_IN();
	} while (len || n == ENDPOINT0_SIZE);
}

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect)
{
        UENUM = 0;
        if (bit_is_set(UEINTX, RXSTPI)) {
                uint8_t bmRequestType = USB_OUT_read_byte();
                uint8_t bRequest      = USB_OUT_read_byte();
                uint16_t wValue  = USB_OUT_read_word();
                uint16_t wIndex  = USB_OUT_read_word();
                uint16_t wLength = USB_OUT_read_word();
		USB_ack_OUT();
		/* acknowledge setup after reading, because it clears the bank */
		USB_ack_SETUP();
                if (bRequest == GET_DESCRIPTOR) {
			serve_get_descriptor(wValue, wIndex, wLength);
			return;
                } else if (bRequest == SET_ADDRESS && bmRequestType == 0) {
			USB_ack_IN();
			USB_wait_IN();
			UDADDR = wValue | _BV(ADDEN);
			return;
		} else if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
			usb_configuration = wValue;
			USB_ack_IN();
			USB_configure_endpoint(KEYBOARD_ENDPOINT,
					EP_TYPE_INTERRUPT_IN,
					EP_SIZE_32 | EP_DOUBLE_BUFFER,
					0x00);
			USB_reset_endpoint_fifo(KEYBOARD_ENDPOINT);
			return;
		} else if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
			USB_wait_IN();
			USB_IN_write_byte(usb_configuration);
			USB_ack_IN();
			return;
		} else if (bRequest == GET_STATUS) {
			USB_wait_IN();
			int i = 0;
			#ifdef SUPPORT_ENDPOINT_HALT
			if (bmRequestType == 0x82) {
				UENUM = wIndex;
				if (UECONX & _BV(STALLRQ)) i = 1;
				UENUM = 0;
			}
			#endif
			USB_IN_write_byte(i);
			USB_IN_write_byte(0);
			USB_ack_IN();
			return;
		}
		#ifdef SUPPORT_ENDPOINT_HALT
		else if ((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
		  && bmRequestType == 0x02 && wValue == 0) {
			int i = wIndex & 0x7F;
			if (i >= 1 && i <= MAX_ENDPOINT) {
				USB_ack_IN();
				UENUM = i;
				if (bRequest == SET_FEATURE) {
					UECONX = _BV(STALLRQ)|_BV(EPEN);
				} else {
					UECONX = _BV(STALLRQC)|_BV(RSTDT)|_BV(EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		#endif
		else if (wIndex == KEYBOARD_INTERFACE) {
			if (bmRequestType == 0xA1) {
				if (bRequest == HID_GET_REPORT) {
					USB_wait_IN();
					USB_IN_write_byte(keyboard_modifier_keys);
					USB_IN_write_byte(0);
					for (int i=0; i<30; i++)
						USB_IN_write_byte(keyboard_keys[i]);
					USB_ack_IN();
					return;
				}
				if (bRequest == HID_GET_IDLE) {
					USB_wait_IN();
					USB_IN_write_byte(keyboard_idle_config);
					USB_ack_IN();
					return;
				}
				if (bRequest == HID_GET_PROTOCOL) {
					USB_wait_IN();
					USB_IN_write_byte(keyboard_protocol);
					USB_ack_IN();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if (bRequest == HID_SET_REPORT) {
					USB_wait_OUT();
					keyboard_leds = USB_OUT_read_byte();
					usb_ack_out();
					USB_ack_IN();
					return;
				}
				if (bRequest == HID_SET_IDLE) {
					keyboard_idle_config = (wValue >> 8);
					keyboard_idle_count = 0;
					USB_ack_IN();
					return;
				}
				if (bRequest == HID_SET_PROTOCOL) {
					keyboard_protocol = wValue;
					USB_ack_IN();
					return;
				}
			}
		}
	}
	UECONX |= _BV(STALLRQ);	// stall
}



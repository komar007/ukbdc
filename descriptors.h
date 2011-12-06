#pragma once

#include <stdint.h>
#include <avr/pgmspace.h>

/* Do not include this file anywhere else than usb.c */

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
/* The following HID descriptors' format is described in
 * Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60 */

/* This descriptor makes sure no reserved keycodes are send to the host by
 * putting constant data in all reserved places. For some reason, however, it
 * doesn't work on Windows. Probably it is a problem with the descriptor, so
 * it needs reviewing. Below it, is a simpler 256-bit descriptor. */
/*
static uint8_t PROGMEM keyboard_hid_report_desc[] = {
        0x05, 0x01,          // Usage Page (Generic Desktop),
        0x09, 0x06,          // Usage (Keyboard),
        0xA1, 0x01,          // Collection (Application),
        0x75, 0x01,          //   Report Size (1),
        0x95, 0x01,          //   Report Count (1),
        0x81, 0x03,          //   Input (Constant),                 ; Reserved bit for usage id 00
        0x05, 0x07,          //   Usage Page (Key Codes),
        0x15, 0x00,          //   Logical Minimum (0),
        0x25, 0x01,          //   Logical Maximum (1),
        0x95, 0xA4,          //   Report Count (164),
        0x19, 0x01,          //   Usage Minimum (1),
        0x29, 0xA4,          //   Usage Maximum (164),
        0x81, 0x02,          //   Input (Data, Variable, Absolute)
        0x95, 0x0B,          //   Report Count (11),
        0x81, 0x03,          //   Input (Constant),                 ; Reserved 11 bits for usage id A5-CF
        0x95, 0x2E,          //   Report Count (46),
        0x19, 0xB0,          //   Usage Minimum (176),
        0x29, 0xDD,          //   Usage Maximum (221),
        0x81, 0x02,          //   Input (Data, Variable, Absolute)
        0x95, 0x02,          //   Report Count (2),
        0x81, 0x03,          //   Input (Constant),                 ; Reserved 2 bits for usage id A5-CF
        0x95, 0x08,          //   Report Count (8),
        0x19, 0xE0,          //   Usage Minimum (224),
        0x29, 0xE7,          //   Usage Maximum (231),
        0x81, 0x02,          //   Input (Data, Variable, Absolute)
        0x95, 0x18,          //   Report Count (24),
        0x81, 0x03,          //   Input (Constant),                 ; 24 bits of padding to full 32 bytes
        0x95, 0x05,          //   Report Count (5),
        0x05, 0x08,          //   Usage Page (LEDs),
        0x19, 0x01,          //   Usage Minimum (1),
        0x29, 0x05,          //   Usage Maximum (5),
        0x91, 0x02,          //   Output (Data, Variable, Absolute), ;LED report
        0x95, 0x01,          //   Report Count (3),
        0x91, 0x03,          //   Output (Constant),                 ;LED report padding
        0xc0                 // End Collection
};
*/
/* Simple 256-bit bitmap HID report descriptor. Bit on position i represents
 * the state of key number i. This allows for reserved 1-byte keycodes to be
 * sent to the host, which is not standard-compliant, so care has to be taken
 * not to set these bits if firmware */
static uint8_t PROGMEM keyboard_hid_report_desc[] = {
        0x05, 0x01,          // Usage Page (Generic Desktop),
        0x09, 0x06,          // Usage (Keyboard),
        0xA1, 0x01,          // Collection (Application),
        0x75, 0x01,          //   Report Size (1),
        0x96, 0x00,0x01,          //   Report Count (256),
        0x05, 0x07,          //   Usage Page (Key Codes),
        0x19, 0x00,          //   Usage Minimum (0),
        0x29, 0xff,          //   Usage Maximum (255),
        0x15, 0x00,          //   Logical Minimum (0),
        0x25, 0x01,          //   Logical Maximum (1),
        0x81, 0x02,          //   Input (Data, Variable, Absolute), ;Modifier byte
        0x95, 0x05,          //   Report Count (5),
        0x75, 0x01,          //   Report Size (1),
        0x05, 0x08,          //   Usage Page (LEDs),
        0x19, 0x01,          //   Usage Minimum (1),
        0x29, 0x05,          //   Usage Maximum (5),
        0x91, 0x02,          //   Output (Data, Variable, Absolute), ;LED report
        0x95, 0x01,          //   Report Count (1),
        0x75, 0x03,          //   Report Size (3),
        0x91, 0x03,          //   Output (Constant),                 ;LED report padding
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

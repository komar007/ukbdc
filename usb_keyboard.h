#pragma once

/* #define CLOCK_8MHZ  for USB operation from 8MHz  crystal */
/* #define CLOCK_16MHZ for USB operation from 16MHz crystal */
#define CLOCK_16MHZ

#include <stdint.h>
#include <stdbool.h>

void usb_init(void);			// initialize everything
uint8_t usb_configured(void);		// is the USB port configured

int8_t usb_keyboard_send(void);
extern uint8_t keyboard_modifier_keys;
extern uint8_t keyboard_keys[30];
extern volatile uint8_t keyboard_leds;

// This file does not include the HID debug functions, so these empty
// macros replace them with nothing, so users can compile code that
// has calls to these functions.
#define usb_debug_putchar(c)
#define usb_debug_flush_output()


#define KEY_CTRL	0x01
#define KEY_SHIFT	0x02
#define KEY_ALT		0x04
#define KEY_GUI		0x08
#define KEY_LEFT_CTRL	0x01
#define KEY_LEFT_SHIFT	0x02
#define KEY_LEFT_ALT	0x04
#define KEY_LEFT_GUI	0x08
#define KEY_RIGHT_CTRL	0x10
#define KEY_RIGHT_SHIFT	0x20
#define KEY_RIGHT_ALT	0x40
#define KEY_RIGHT_GUI	0x80

#define KEY_A		4
#define KEY_B		5
#define KEY_C		6
#define KEY_D		7
#define KEY_E		8
#define KEY_F		9
#define KEY_G		10
#define KEY_H		11
#define KEY_I		12
#define KEY_J		13
#define KEY_K		14
#define KEY_L		15
#define KEY_M		16
#define KEY_N		17
#define KEY_O		18
#define KEY_P		19
#define KEY_Q		20
#define KEY_R		21
#define KEY_S		22
#define KEY_T		23
#define KEY_U		24
#define KEY_V		25
#define KEY_W		26
#define KEY_X		27
#define KEY_Y		28
#define KEY_Z		29
#define KEY_1		30
#define KEY_2		31
#define KEY_3		32
#define KEY_4		33
#define KEY_5		34
#define KEY_6		35
#define KEY_7		36
#define KEY_8		37
#define KEY_9		38
#define KEY_0		39
#define KEY_ENTER	40
#define KEY_ESC		41
#define KEY_BACKSPACE	42
#define KEY_TAB		43
#define KEY_SPACE	44
#define KEY_MINUS	45
#define KEY_EQUAL	46
#define KEY_LEFT_BRACE	47
#define KEY_RIGHT_BRACE	48
#define KEY_BACKSLASH	49
#define KEY_NUMBER	50
#define KEY_SEMICOLON	51
#define KEY_QUOTE	52
#define KEY_TILDE	53
#define KEY_COMMA	54
#define KEY_PERIOD	55
#define KEY_SLASH	56
#define KEY_CAPS_LOCK	57
#define KEY_F1		58
#define KEY_F2		59
#define KEY_F3		60
#define KEY_F4		61
#define KEY_F5		62
#define KEY_F6		63
#define KEY_F7		64
#define KEY_F8		65
#define KEY_F9		66
#define KEY_F10		67
#define KEY_F11		68
#define KEY_F12		69
#define KEY_PRINTSCREEN	70
#define KEY_SCROLL_LOCK	71
#define KEY_PAUSE	72
#define KEY_INSERT	73
#define KEY_HOME	74
#define KEY_PAGE_UP	75
#define KEY_DELETE	76
#define KEY_END		77
#define KEY_PAGE_DOWN	78
#define KEY_RIGHT	79
#define KEY_LEFT	80
#define KEY_DOWN	81
#define KEY_UP		82
#define KEY_NUM_LOCK	83
#define KEYPAD_SLASH	84
#define KEYPAD_ASTERIX	85
#define KEYPAD_MINUS	86
#define KEYPAD_PLUS	87
#define KEYPAD_ENTER	88
#define KEYPAD_1	89
#define KEYPAD_2	90
#define KEYPAD_3	91
#define KEYPAD_4	92
#define KEYPAD_5	93
#define KEYPAD_6	94
#define KEYPAD_7	95
#define KEYPAD_8	96
#define KEYPAD_9	97
#define KEYPAD_0	98
#define KEYPAD_PERIOD	99




// Everything below this point is only intended for usb_serial.c
#ifdef USB_SERIAL_PRIVATE_INCLUDE
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

/* [FLAGS FOR UECFG0X] */
/* Endpoint types and dirs (according to atmel specification) */
#define EP_TYPE_CONTROL			0x00
#define EP_TYPE_BULK_IN			0x81
#define EP_TYPE_BULK_OUT		0x80
#define EP_TYPE_INTERRUPT_IN		0xC1
#define EP_TYPE_INTERRUPT_OUT		0xC0
#define EP_TYPE_ISOCHRONOUS_IN		0x41
#define EP_TYPE_ISOCHRONOUS_OUT		0x40
/* [/FLAGS FOR UECFG0X] */

/* [FLAGS FOR UECFG1X] */
/* the following bank configs include the ALLOC bit! */
#define EP_SINGLE_BUFFER		(0x00 | _BV(ALLOC))
#define EP_DOUBLE_BUFFER		(0x04 | _BV(ALLOC))

#define EP_SIZE_8			0x00
#define EP_SIZE_16			0x10
#define EP_SIZE_32			0x20
#define EP_SIZE_64			0x30
#define EP_SIZE_128			0x40
#define EP_SIZE_256			0x50
#define EP_SIZE_512			0x60
/* [/FLAGS FOR UECFG1X] */

#define EP_SIZE(s)	((s) == 128? 0x40 :	\
			((s) == 64 ? 0x30 :	\
			((s) == 32 ? 0x20 :	\
			((s) == 16 ? 0x10 :	\
			             0x00))))

#define MAX_ENDPOINT		4

#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)

static inline bool PLL_is_locked()
{
	return bit_is_set(PLLCSR, PLOCK);
}

/* deprecated, probably not necessary at all; make sure it's not needed by
 * mcus other than mega32u4
#define USB_FREEZE() (USBCON = (_BV(USBE) | _BV(FRZCLK)))
 */

/* enable or disable USB pad regulator */
static inline void USB_set_pads_regulator(bool state)
{
#if defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	/* make sure the controller is in device mode */
	UHWCON |= _BV(UIMOD);
#endif
/* AT90UB162 does not have UHWCON */
#if ! defined(__AVR_AT90USB162__)
	if (state)
		UHWCON |= _BV(UVREGE);
	else
		UHWCON &= ~_BV(UVREGE);
#endif
}

enum Crystal {
	CRYSTAL_16MHZ,
	CRYSTAL_8MHZ
};
static inline void USB_pll_config(enum Crystal crystal)
{
	switch (crystal) {
	case CRYSTAL_16MHZ:
#if defined(__AVR_ATmega32U4__)
		/* set PLL input prescaler to 1:2 */
		PLLCSR |= _BV(PINDIV);
#elif defined(__AVR_AT90USB646__)
		/* set PLL input prescaler to 1:8 */
		PLLCSR = _BV(PLLP2) | _BV(PLLP1);
#elif defined(__AVR_AT90USB1286__)
		/* set PLL input prescaler to 1:8 (other config than for AT90USB646) */
		PLLCSR = _BV(PLLP2) | _BV(PLLP0);
#elif defined(__AVR_AT90USB162__)
		/* set PLL input prescaler to 1:2 */
		PLLCSR = _BV(PLLP0);
#endif
		break;
	case CRYSTAL_8MHZ:
#if defined(__AVR_ATmega32U4__)
		/* set PLL input prescaler to 1:1 */
		PLLCSR &= ~_BV(PINDIV);
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
		/* set PLL input prescaler to 1:4 */
		PLLCSR = _BV(PLLP1) | _BV(PLLP0);
#elif defined(__AVR_AT90USB162__)
		/* set PLL input prescaler to 1:1 */
		PLLCSR = 0;
#endif
	default:
		break;
	}
	PLLCSR |= _BV(PLLE);
}

static inline void USB_enable()
{
#if defined(__AVR_AT90USB162__)
	/* USB macro enable */
	USBCON |= _BV(USBE);
	/* unfreeze USB clock */
	USBCON &= ~_BV(FRZCLK);
#else
	/* USB macro enable and pad enable */
	USBCON |= _BV(USBE) | _BV(OTGPADE);
	/* unfreeze USB clock, disable VBUS transition interrupt */
	USBCON &= ~(_BV(FRZCLK) | _BV(VBUSTE));
#endif
}

static inline void USB_configure_endpoint(uint8_t endp_n,
		uint8_t type, uint8_t config, uint8_t inte)
{
	UENUM   = endp_n;
	UECONX  = _BV(EPEN);
	UECFG0X = type;
	UECFG1X = config;
	/* set interrupt enable bits */
	UEIENX  = inte;
}

static inline void USB_reset_endpoint_fifo(uint8_t endp_n)
{
	UERST = _BV(endp_n);
	UERST = 0x00;
}

/* Acknowledge reception of SETUP packet */
static inline void USB_ack_SETUP()
{
	UEINTX &= ~_BV(RXSTPI);
}

/* Wait until OUT transaction data is received from host and ready in buffer */
static inline void USB_wait_OUT()
{
	while (bit_is_clear(UEINTX, RXOUTI))
		;
}
/* Read a byte from OUT transaction data in buffer */
static inline uint8_t USB_OUT_read_byte()
{
	return UEDATX;
}
/* Read a word from OUT transaction data in buffer */
static inline uint16_t USB_OUT_read_word()
{
	uint8_t low_byte = UEDATX;
	return (UEDATX << 8) | low_byte;
}
/* Acknowledge OUT transaction (after reading with USB_OUT_read_*) */
static inline void USB_ack_OUT()
{
	UEINTX &= ~_BV(RXOUTI);
}

/* Wait until IN transaction can be started with the host (wait for IN token) */
static inline void USB_wait_IN()
{
	while (bit_is_clear(UEINTX, TXINI))
		;
}
/* Write a byte of data to an IN transaction buffer */
static inline void USB_IN_write_byte(uint8_t byte)
{
	UEDATX = byte;
}
/* Flush an IN transaction (after writing with USB_IN_write_*) */
static inline void USB_flush_IN()
{
	UEINTX &= ~_BV(TXINI);
}

struct setup_packet {
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};

static inline void USB_control_write_complete_status_stage()
{
	USB_wait_IN();
	/* send Zero Length Packet */
	USB_flush_IN();
}

static inline void USB_control_read_complete_status_stage()
{
	/* wait for Zero Length Packet from host */
	USB_wait_OUT();
	/* acknowledge it */
	USB_ack_OUT();
}

/* Set device's address */
static inline void USB_set_addr(uint8_t addr)
{
	UDADDR = addr;
}
/* Make the controller start using the address configured by USB_set_addr */
static inline void USB_addr_enable()
{
	UDADDR |= _BV(ADDEN);
}


// standard control endpoint request types
#define GET_STATUS			0
#define CLEAR_FEATURE			1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR			6
#define GET_CONFIGURATION		8
#define SET_CONFIGURATION		9
#define GET_INTERFACE			10
#define SET_INTERFACE			11
// HID (human interface device)
#define HID_GET_REPORT			1
#define HID_GET_IDLE			2
#define HID_GET_PROTOCOL		3
#define HID_SET_REPORT			9
#define HID_SET_IDLE			10
#define HID_SET_PROTOCOL		11
// CDC (communication class device)
#define CDC_SET_LINE_CODING		0x20
#define CDC_GET_LINE_CODING		0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22
#endif

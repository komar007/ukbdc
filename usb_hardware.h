/* This file contains functions which manipulate the AVR's USB controller by
 * writing to SFRs and reading SFRs. It also contains definitions of
 * configuration parameters which are specific to AVR's USB controller and not
 * connected with the USB standard */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

/* check if the PLL has locked to the clock */
static inline bool PLL_is_locked()
{
	return bit_is_set(PLLCSR, PLOCK);
}

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

enum crystal_config {
	CRYSTAL_16MHZ,
	CRYSTAL_8MHZ
};
static inline void USB_pll_config(enum crystal_config crystal)
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

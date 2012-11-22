#include "platforms.h"

struct pin_config PINS[NUM_IO] = {
#ifdef PLATFORM_alpha
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD7)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD6)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD5)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD4)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD3)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD2)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD1)},
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD0)},

	{.portx = &PORTE, .pinx = &PINE, .ddrx = &DDRE, .mask = _BV(PE6)},

	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF7)},
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF6)},
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF5)},
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF4)},
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF1)},
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF0)},

	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB7)},
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB6)},
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB5)},
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB4)},
#endif
};

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

#ifdef PLATFORM_gh60
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD0)}, // r1
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD1)}, // r2
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD2)}, // r3
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD3)}, // r4
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD5)}, // r5

	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF0)}, // c1
	{.portx = &PORTF, .pinx = &PINF, .ddrx = &DDRF, .mask = _BV(PF1)}, // c2
	{.portx = &PORTE, .pinx = &PINE, .ddrx = &DDRE, .mask = _BV(PE6)}, // c3
	{.portx = &PORTC, .pinx = &PINC, .ddrx = &DDRC, .mask = _BV(PC7)}, // c4
	{.portx = &PORTC, .pinx = &PINC, .ddrx = &DDRC, .mask = _BV(PC6)}, // c5
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB6)}, // c6
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD4)}, // c7
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB1)}, // c8
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB0)}, // c9
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB5)}, // c10
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB4)}, // c11
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD7)}, // c12
	{.portx = &PORTD, .pinx = &PIND, .ddrx = &DDRD, .mask = _BV(PD6)}, // c13
	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB3)}, // c14

	{.portx = &PORTB, .pinx = &PINB, .ddrx = &DDRB, .mask = _BV(PB2)}  // caps
#endif
};

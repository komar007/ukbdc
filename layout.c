#include "layout.h"

static uint8_t *current_layout = NULL;

/* Sets current layout. Takes ownership of data passed through the pointer */
void LAYOUT_set(uint8_t *layout)
{
	free(current_layout);
	current_layout = layout;
}

/* Returns the scancode associated with a keycode */
uint8_t LAYOUT_get_scancode(uint8_t layer, uint8_t keycode)
{
	return current_layout[layer*61 + keycode];
}

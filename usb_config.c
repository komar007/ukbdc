#include "usb_config.h"

#include "hid.h"

struct interface_request_handler iface_req_handlers[] = {
	{.iface_number = KEYBOARD_INTERFACE, .f = &HID_handle_control_request},
	{.iface_number = 0, .f = 0}
};

struct sof_handler sof_handlers[] = {
	{.f = &HID_handle_sof},
	{.f = 0}
};

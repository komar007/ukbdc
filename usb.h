/* [FLAGS FOR bmRequestType] */
/* Request direction */
#define REQUEST_DIRECTION	0x80 /* mask */
#define HOST_TO_DEVICE		0x00
#define DEVICE_TO_HOST		0x80
/* Request type */
#define REQUEST_TYPE		0x60 /* mask */
#define STANDARD		0x00
#define CLASS			0x20
#define VENDOR			0x40
/* Request recipient */
#define REQUEST_RECIPIENT	0x1F /* mask */
#define DEVICE			0x00
#define INTERFACE		0x01
#define ENDPOINT		0x02
#define OTHER			0x03
/* [/FLAGS FOR bmRequestType] */

static inline bool is_request_type(uint8_t bmRequestType,
		uint8_t mask, uint8_t flags)
{
	return (bmRequestType & mask) == flags;
}

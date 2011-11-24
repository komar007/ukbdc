/* [FLAGS FOR bmRequestType] */
/* Request direction */
#define MASK_REQ_DIR		0x80
#define HOST_TO_DEVICE		0x00
#define DEVICE_TO_HOST		0x80
/* Request type */
#define MASK_REQ_TYPE		0x60
#define STANDARD		0x00
#define CLASS			0x20
#define VENDOR			0x40
/* Request recipient */
#define MASK_REQ_RCPT		0x1F
#define DEVICE			0x00
#define INTERFACE		0x01
#define ENDPOINT		0x02
#define OTHER			0x03

#define MASK_ALL 		(MASK_REQ_DIR | MASK_REQ_TYPE | MASK_REQ_RCPT)
/* [/FLAGS FOR bmRequestType] */

static inline bool is_request_type(uint8_t bmRequestType,
		uint8_t mask, uint8_t flags)
{
	return (bmRequestType & mask) == flags;
}

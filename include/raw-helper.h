#ifndef RAW_HELPER_H
#define RAW_HELPER_H

#include <stdint.h>
#include <stdbool.h>

//#include <linux/hid.h>
#include <linux/usb/ch9.h>

#include <libusb.h>

/*----------------------------------------------------------------------*/

struct hid_class_descriptor {
	__u8  bDescriptorType;
	__le16 wDescriptorLength;
} __attribute__ ((packed));

struct hid_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__le16 bcdHID;
	__u8  bCountryCode;
	__u8  bNumDescriptors;

	struct hid_class_descriptor desc[1];
} __attribute__ ((packed));

struct audio_control_descriptor_1 {	// HEADER
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__le16 bcdADC;
	__le16 wTotalLength;
	__u8  bInCollection;
	__u8  baInterfaceNr0;
	__u8  baInterfaceNr1;
} __attribute__ ((packed));

struct audio_control_descriptor_2 {	// INPUT_TERMINAL
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bTerminalID;
	__le16  wTerminalType;

	__u8 bAssocTerminal;
	__u8 bNrChannels;
	__le16  wChannelConfig;
	__u8  iChannelNames;
	__u8  iTerminal;
} __attribute__ ((packed));

struct audio_control_descriptor_6 { // FEATURE_UNIT
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bUnitID;
	__u8 bSourceID;
	__u8 bControlSize;
	__u8 bmaControls0;
	__u8 bmaControls1;
	__u8 bmaControls2;
	__u8 bmaControls3;
	__u8 bmaControls4;
	__u8 iFeature;
} __attribute__ ((packed));

struct audio_control_descriptor_3 { // OUTPUT_TERMINAL
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bTerminalID;
	__le16  wTerminalType;

	__u8 bAssocTerminal;
	__u8 bSourceID;
	__u8  iTerminal;
} __attribute__ ((packed));

/*----------------------------------------------------------------------*/

#define UDC_NAME_LENGTH_MAX 128

struct usb_raw_init {
	__u8 driver_name[UDC_NAME_LENGTH_MAX];
	__u8 device_name[UDC_NAME_LENGTH_MAX];
	__u8 speed;
};

enum usb_raw_event_type {
	USB_RAW_EVENT_INVALID,
	USB_RAW_EVENT_CONNECT,
	USB_RAW_EVENT_CONTROL,
};

struct usb_raw_event {
	__u32		type;
	__u32		length;
	__u8		data[0];
//	__u8*		data;
};

struct usb_raw_ep_io {
	__u16		ep;
	__u16		flags;
	__u32		length;
		__u8		data[0];
//	 __u8*		data;
};

#define USB_RAW_EPS_NUM_MAX	30
#define USB_RAW_EP_NAME_MAX	16
#define USB_RAW_EP_ADDR_ANY	0xff

struct usb_raw_ep_caps {
	__u32	type_control	: 1;
	__u32	type_iso	: 1;
	__u32	type_bulk	: 1;
	__u32	type_int	: 1;
	__u32	dir_in		: 1;
	__u32	dir_out		: 1;
};

struct usb_raw_ep_limits {
	__u16	maxpacket_limit;
	__u16	max_streams;
	__u32	reserved;
};

struct usb_raw_ep_info {
	__u8				name[USB_RAW_EP_NAME_MAX];
	__u32				addr;
	struct usb_raw_ep_caps		caps;
	struct usb_raw_ep_limits	limits;
};

struct usb_raw_eps_info {
	struct usb_raw_ep_info	eps[USB_RAW_EPS_NUM_MAX];
};

#define USB_RAW_IOCTL_INIT		_IOW('U', 0, struct usb_raw_init)
#define USB_RAW_IOCTL_RUN		_IO('U', 1)
#define USB_RAW_IOCTL_EVENT_FETCH	_IOR('U', 2, struct usb_raw_event)
#define USB_RAW_IOCTL_EP0_WRITE		_IOW('U', 3, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP0_READ		_IOWR('U', 4, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP_ENABLE		_IOW('U', 5, struct usb_endpoint_descriptor)
#define USB_RAW_IOCTL_EP_DISABLE	_IOW('U', 6, __u32)
#define USB_RAW_IOCTL_EP_WRITE		_IOW('U', 7, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_EP_READ		_IOWR('U', 8, struct usb_raw_ep_io)
#define USB_RAW_IOCTL_CONFIGURE		_IO('U', 9)
#define USB_RAW_IOCTL_VBUS_DRAW		_IOW('U', 10, __u32)
#define USB_RAW_IOCTL_EPS_INFO		_IOR('U', 11, struct usb_raw_eps_info)
#define USB_RAW_IOCTL_EP0_STALL		_IO('U', 12)
#define USB_RAW_IOCTL_EP_SET_HALT	_IOW('U', 13, __u32)
#define USB_RAW_IOCTL_EP_CLEAR_HALT	_IOW('U', 14, __u32)
#define USB_RAW_IOCTL_EP_SET_WEDGE	_IOW('U', 15, __u32)

/*----------------------------------------------------------------------*/

#define EP0_MAX_DATA 512	// increased to 512 due to size 507 ds4 report descriptor
#define EP_MAX_PACKET_INT	1024

struct usb_raw_control_event {
	struct usb_raw_event		inner;
	struct usb_ctrlrequest		ctrl;
};

struct usb_raw_control_io {
	struct usb_raw_ep_io		inner;
	char				data[EP0_MAX_DATA];
};

struct usb_raw_int_io {
	struct usb_raw_ep_io		inner;
	char				data[EP_MAX_PACKET_INT];
};

/*----------------------------------------------------------------------*/
typedef struct {
	int fd;	// raw_gadget descriptor
	int ep_int;// endpoint handler
	libusb_device_handle *deviceHandle;
	bool keepRunning;	// thread management, mostly unused
	bool stop;	// for endpoint termination from interface switching
	int busyPackets; // to notice then EP is safe to be diasbled
	
	struct usb_endpoint_descriptor usb_endpoint;
	int bIntervalInMicroseconds;
	unsigned char* data;
} EndpointInfo;

typedef struct {
//	bool active;
	uint8_t bInterfaceNumber;
	int bNumEndpoints;
	EndpointInfo *mEndpointInfos;
	
} AlternateInfo;

typedef struct {
//	bool active;
	int activeAlternate;
	int bNumAlternates;
	AlternateInfo *mAlternateInfos;
	
} InterfaceInfo;

typedef struct {
//	bool active;
	int activeInterface;
	int bNumInterfaces;
	InterfaceInfo *mInterfaceInfos;
	
} ConfigurationInfo;

typedef struct {
//	int totalEndpoints;
//	EndpointInfo *mEndpointInfos;
//	InterfaceInfo *mInterfaceInfos;
	int bNumConfigurations;
	int activeConfiguration;
	ConfigurationInfo* mConfigurationInfos;
	int fd;
	libusb_device_handle *dev_handle;
	//struct libusb_config_descriptor* configDescriptor;
} EndpointZeroInfo;

/*----------------------------------------------------------------------*/
bool assign_ep_address(struct usb_raw_ep_info *info, struct usb_endpoint_descriptor *ep);
void process_eps_info(EndpointZeroInfo* epZeroInfo);

/*----------------------------------------------------------------------*/

int usb_raw_open();
void usb_raw_init(int fd, enum usb_device_speed speed, const char *driver, const char *device);
void usb_raw_run(int fd);
void usb_raw_event_fetch(int fd, struct usb_raw_event *event);

int usb_raw_ep0_read(int fd, struct usb_raw_ep_io *io);
int usb_raw_ep0_write(int fd, struct usb_raw_ep_io *io);

int usb_raw_ep_enable(int fd, struct usb_endpoint_descriptor *desc);
int usb_raw_ep_disable(int fd, uint32_t something);
int usb_raw_ep_write(int fd, struct usb_raw_ep_io *io);
int usb_raw_ep_read(int fd, struct usb_raw_ep_io *io);

void usb_raw_configure(int fd);
void usb_raw_vbus_draw(int fd, uint32_t power);

int usb_raw_eps_info(int fd, struct usb_raw_eps_info *info);
void usb_raw_ep0_stall(int fd);
void usb_raw_ep_set_halt(int fd, int ep);

/*----------------------------------------------------------------------*/

void log_control_request(struct usb_ctrlrequest *ctrl);
void log_event(struct usb_raw_event *event);


/*----------------------------------------------------------------------*/
// from another resource
//inline void put_unaligned_le16(__u16 val, __u16 *cp)
//{
//	__u8	*p = (void *)cp;
//
//	*p++ = (__u8) val;
//	*p++ = (__u8) (val >> 8);
//}
int utf8_to_utf16le(const char *s, __u16 *cp, unsigned len);

#endif

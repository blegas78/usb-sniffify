// SPDX-License-Identifier: GPL-2.0-only

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
//#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/hid.h>
//#include <linux/usb/ch9.h>

#include <pthread.h>

//#include "ds4.h"
#include "dualsense.h"
//#include "magicPro.h"



#include "raw-helper.h"


/*----------------------------------------------------------------------*/

//#define BCD_USB		0x0200
//
////#define USB_VENDOR	0x1d6b
////#define USB_PRODUCT	0x0104
////#define USB_VENDOR	0x0525
////#define USB_PRODUCT	0xa4a0
//#define USB_VENDOR	0x054c
//#define USB_PRODUCT	0x09cc
//
//#define STRING_ID_MANUFACTURER	1
//#define STRING_ID_PRODUCT	2
//#define STRING_ID_SERIAL	0
//#define STRING_ID_CONFIG	0
//#define STRING_ID_INTERFACE	0
//
//#define EP_MAX_PACKET_CONTROL	64
//#define EP_MAX_PACKET_INT	64
//
//// Assigned dynamically.
//#define EP_NUM_INT_IN	0x0

struct usb_device_descriptor usb_device = {
	.bLength =		USB_DT_DEVICE_SIZE,
	.bDescriptorType =	USB_DT_DEVICE,
	.bcdUSB =		__constant_cpu_to_le16(BCD_USB),
	.bDeviceClass =		B_DEVICE_CLASS,
	.bDeviceSubClass =	B_DEVICE_SUBCLASS,
	.bDeviceProtocol =	B_DEVICE_PROTOCOL,
	.bMaxPacketSize0 =	EP_MAX_PACKET_CONTROL,
	.idVendor =		__constant_cpu_to_le16(USB_VENDOR),
	.idProduct =		__constant_cpu_to_le16(USB_PRODUCT),
	.bcdDevice =		BCD_DEVICE,
	.iManufacturer =	STRING_ID_MANUFACTURER,
	.iProduct =		STRING_ID_PRODUCT,
	.iSerialNumber =	STRING_ID_SERIAL,
	.bNumConfigurations =	NUM_COFIGURATIONS,
};

struct usb_qualifier_descriptor usb_qualifier = {
	.bLength =		sizeof(struct usb_qualifier_descriptor),
	.bDescriptorType =	USB_DT_DEVICE_QUALIFIER,
	.bcdUSB =		__constant_cpu_to_le16(BCD_USB),
	.bDeviceClass =		0,
	.bDeviceSubClass =	0,
	.bDeviceProtocol =	0,
	.bMaxPacketSize0 =	EP_MAX_PACKET_CONTROL,
	.bNumConfigurations =	1,
	.bRESERVED =		0,
};

struct usb_config_descriptor usb_config = {
	.bLength =		USB_DT_CONFIG_SIZE,
	.bDescriptorType =	USB_DT_CONFIG,
	.wTotalLength =		0,  // computed later
	.bNumInterfaces =	B_NUM_INTERFACES,
	.bConfigurationValue = B_CONFIGURATION_VALUE,
	.iConfiguration = 	STRING_ID_CONFIG,
	.bmAttributes =		USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower =		B_MAX_POWER,
};

#define NUM_ENDPOINTS 2

struct usb_interface_descriptor usb_interface_audio1 = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bInterfaceNumber =	0,
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_CLASS_AUDIO, //USB_CLASS_HID,
	.bInterfaceSubClass =	1, // Control DEvice
	.bInterfaceProtocol =	0,
	.iInterface =		0,
};



struct usb_interface_descriptor usb_interface = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bInterfaceNumber =	3,
	.bAlternateSetting =	0,
	.bNumEndpoints =	NUM_ENDPOINTS,
	.bInterfaceClass =	USB_CLASS_HID,
	.bInterfaceSubClass =	0,
	.bInterfaceProtocol =	0,
	.iInterface =		STRING_ID_INTERFACE,
};

//struct usb_endpoint_descriptor usb_endpoints[NUM_ENDPOINTS] = {
//	{
//		.bLength =		USB_DT_ENDPOINT_SIZE,
//		.bDescriptorType =	USB_DT_ENDPOINT,
//		.bEndpointAddress =	USB_DIR_IN | EP_NUM_INT_IN | 0X04,
//		.bmAttributes =		USB_ENDPOINT_XFER_INT,
//		.wMaxPacketSize =	EP_MAX_PACKET_INT,
//		.bInterval =		B_INTERVAL,
//	}, {
//		.bLength =		USB_DT_ENDPOINT_SIZE,
//		.bDescriptorType =	USB_DT_ENDPOINT,
//		.bEndpointAddress =	USB_DIR_OUT | EP_NUM_INT_IN | 0x03,
//		.bmAttributes =		USB_ENDPOINT_XFER_INT,
//		.wMaxPacketSize =	EP_MAX_PACKET_INT,
//		.bInterval =		B_INTERVAL,
//	}
//};

struct usb_endpoint_descriptor usb_endpoints[4] = {
	{
		.bLength =		USB_DT_ENDPOINT_SIZE,
		.bDescriptorType =	USB_DT_ENDPOINT,
		.bEndpointAddress =	USB_DIR_IN | EP_NUM_INT_IN | 0X04,
		.bmAttributes =		USB_ENDPOINT_XFER_INT,
		.wMaxPacketSize =	EP_MAX_PACKET_INT,
		.bInterval =		B_INTERVAL,
	}, {
		.bLength =		USB_DT_ENDPOINT_SIZE,
		.bDescriptorType =	USB_DT_ENDPOINT,
		.bEndpointAddress =	USB_DIR_OUT | EP_NUM_INT_IN | 0x03,
		.bmAttributes =		USB_ENDPOINT_XFER_INT,
		.wMaxPacketSize =	EP_MAX_PACKET_INT,
		.bInterval =		B_INTERVAL,
	}, {
		.bLength =		9,
		.bDescriptorType =	5,
		.bEndpointAddress =	USB_DIR_IN | EP_NUM_INT_IN | 0X02,
		.bmAttributes =		5,
		.wMaxPacketSize =	0x00c4,
		.bInterval =		4,
	}, {
		.bLength =		9,
		.bDescriptorType =	5,
		.bEndpointAddress =	USB_DIR_OUT | EP_NUM_INT_IN | 0x01,
		.bmAttributes =		9,
		.wMaxPacketSize =	0x0188,
		.bInterval =		4,
	}
};



struct hid_descriptor usb_hid = {
	.bLength =		9,
	.bDescriptorType =	HID_DT_HID,
	.bcdHID =		__constant_cpu_to_le16(0x0111),
	.bCountryCode =		0,
	.bNumDescriptors =	1,
	.desc =			{
		{
			.bDescriptorType =	HID_DT_REPORT,
			.wDescriptorLength =	sizeof(usb_hid_report),
		}
	},
};

int build_config(char *data, int length) {
//	struct usb_config_descriptor *config =
//		(struct usb_config_descriptor *)data;
//	int total_length = 0;
//
//	assert(length >= sizeof(usb_config));
//	memcpy(data, &usb_config, sizeof(usb_config));
//	data += sizeof(usb_config);
//	length -= sizeof(usb_config);
//	total_length += sizeof(usb_config);
//
//	assert(length >= sizeof(usb_interface));
//	memcpy(data, &usb_interface, sizeof(usb_interface));
//	data += sizeof(usb_interface);
//	length -= sizeof(usb_interface);
//	total_length += sizeof(usb_interface);
//
//	assert(length >= sizeof(usb_hid));
//	memcpy(data, &usb_hid, sizeof(usb_hid));
//	data += sizeof(usb_hid);
//	length -= sizeof(usb_hid);
//	total_length += sizeof(usb_hid);
//
//	for (int i = 0; i < usb_interface.bNumEndpoints; i++) {
//		assert(length >= USB_DT_ENDPOINT_SIZE);
//		memcpy(data, &usb_endpoints[i], USB_DT_ENDPOINT_SIZE);
//		data += USB_DT_ENDPOINT_SIZE;
//		length -= USB_DT_ENDPOINT_SIZE;
//		total_length += USB_DT_ENDPOINT_SIZE;
//	}
//
//
////	assert(length >= USB_DT_ENDPOINT_SIZE);
////	memcpy(data, &usb_endpoint2, USB_DT_ENDPOINT_SIZE);
////	data += USB_DT_ENDPOINT_SIZE;
////	length -= USB_DT_ENDPOINT_SIZE;
////	total_length += USB_DT_ENDPOINT_SIZE;
//
//	config->wTotalLength = __cpu_to_le16(total_length);
//	printf("config->wTotalLength: %d\n", total_length);
//
//	return total_length;

	memcpy(data, rawDualsenseConfig, 227);
	return 227;
}

/*----------------------------------------------------------------------*/

bool assign_ep_address(struct usb_raw_ep_info *info,
				struct usb_endpoint_descriptor *ep) {
	if (usb_endpoint_num(ep) != 0)
		return false;  // Already assigned.
	if (usb_endpoint_dir_in(ep) && !info->caps.dir_in)
		return false;
	if (usb_endpoint_dir_out(ep) && !info->caps.dir_out)
		return false;
	switch (usb_endpoint_type(ep)) {
	case USB_ENDPOINT_XFER_BULK:
		if (!info->caps.type_bulk)
			return false;
		break;
	case USB_ENDPOINT_XFER_INT:
		if (!info->caps.type_int)
			return false;
		break;
	default:
		assert(false);
	}
	if (info->addr == USB_RAW_EP_ADDR_ANY) {
		static int addr = 1;
		ep->bEndpointAddress |= addr++;
	} else
		ep->bEndpointAddress |= info->addr;
	return true;
}

void process_eps_info(int fd) {
	struct usb_raw_eps_info info;
	memset(&info, 0, sizeof(info));

	int num = usb_raw_eps_info(fd, &info);
	for (int i = 0; i < num; i++) {
		printf("ep #%d:\n", i);
		printf("  name: %s\n", &info.eps[i].name[0]);
		printf("  addr: %u\n", info.eps[i].addr);
		printf("  type: %s %s %s\n",
			info.eps[i].caps.type_iso ? "iso" : "___",
			info.eps[i].caps.type_bulk ? "blk" : "___",
			info.eps[i].caps.type_int ? "int" : "___");
		printf("  dir : %s %s\n",
			info.eps[i].caps.dir_in ? "in " : "___",
			info.eps[i].caps.dir_out ? "out" : "___");
		printf("  maxpacket_limit: %u\n",
			info.eps[i].limits.maxpacket_limit);
		printf("  max_streams: %u\n", info.eps[i].limits.max_streams);
	}

	for (int e = 0; e < usb_interface.bNumEndpoints; e++) {
		for (int i = 0; i < num; i++) {
			if (assign_ep_address(&info.eps[i], &usb_endpoints[e]))
				continue;
		}
		
		int int_in_addr = usb_endpoint_num(&usb_endpoints[e]);
		assert(int_in_addr != 0);
		printf("int_in: addr = %u\n", int_in_addr);
	}
}






int len;

int ep_int_in = -1;
int ep_int_out = -1;
char dummyBuffer[128];
bool ep0_request(int fd, struct usb_raw_control_event *event,
				 struct usb_raw_control_io *io, bool *done) {
	switch (event->ctrl.bRequestType & USB_TYPE_MASK) {
		case USB_TYPE_STANDARD:
			switch (event->ctrl.bRequest) {
				case USB_REQ_GET_DESCRIPTOR:
					switch (event->ctrl.wValue >> 8) {
						case USB_DT_DEVICE:
							memcpy(&io->data[0], &usb_device, sizeof(usb_device));
							io->inner.length = sizeof(usb_device);
							return true;
						case USB_DT_DEVICE_QUALIFIER:
							memcpy(&io->data[0], &usb_qualifier, sizeof(usb_qualifier));
							io->inner.length = sizeof(usb_qualifier);
							return true;
						case USB_DT_CONFIG:
							io->inner.length = build_config(&io->data[0], sizeof(io->data));
							return true;
						case USB_DT_STRING:
							io->data[0] = 4;
							io->data[1] = USB_DT_STRING;
							switch (event->ctrl.wValue & 0xff) {
								case STRING_ID_MANUFACTURER:
//									memcpy(&io->data[2],"x0",2);
//									io->inner.length = 2+2;
//									io->data[0] = 6;
//									usb_gadget_get_string();
									
										
										len = strlen(manufacturerString);
	//									memcpy(&io->data[2],"Wireless Controller",19);
										memset (io->data + 2, 0, 2 * len);
										len = utf8_to_utf16le(manufacturerString, (__u16 *)&io->data[2], len);
										io->data[0] = (len + 1) * 2;
									break;
//
//
								case STRING_ID_PRODUCT:
									len = strlen(productString);
//									memcpy(&io->data[2],"Wireless Controller",19);
									memset (io->data + 2, 0, 2 * len);
									len = utf8_to_utf16le(productString, (__u16 *)&io->data[2], len);
									//io->inner.length = 19+2;
									io->data[0] = (len + 1) * 2;
									break;
//								case STRING_ID_SERIAL:
								case 0x00:
									io->data[2] = 0x09;
									io->data[3] = 0x04;
									//io->inner.length = 4;
									break;
								default:
									io->data[2] = 'x';
								    io->data[3] = 0x00;
									//io->inner.length = 4;
									break;
							}
							
							io->inner.length = event->ctrl.wLength < io->data[0] ? event->ctrl.wLength : io->data[0];//io->data[0];
							return true;
						case HID_DT_REPORT:
							memcpy(&io->data[0], &usb_hid_report[0],
								   sizeof(usb_hid_report));
							io->inner.length = sizeof(usb_hid_report);
							printf(" ------ sizeof(usb_hid_report) = %d\n", (int) sizeof(usb_hid_report));
							//io->inner.addr = 3;
							//*done = true;
							return true;
						case USB_DT_DEBUG:
							
							return false;
						default:
							printf("fail: event->ctrl.wValue >> 8 no response\n");
							exit(EXIT_FAILURE);
					}
					break;
				case USB_REQ_SET_CONFIGURATION:
					if (ep_int_in  < 0) {
						ep_int_in = usb_raw_ep_enable(fd, &usb_endpoints[0]);
					}
					if (ep_int_out < 0) {
						ep_int_out = usb_raw_ep_enable(fd, &usb_endpoints[1]);
						usb_raw_ep_enable(fd, &usb_endpoints[2]);
						usb_raw_ep_enable(fd, &usb_endpoints[3]);
					}
					printf(" ---- ep_int_in = %d\n", ep_int_in);
					printf(" ---- ep_int_out = %d\n", ep_int_out);
					usb_raw_vbus_draw(fd, usb_config.bMaxPower);
					usb_raw_configure(fd);
					io->inner.length = 0;
					return true;
				case USB_REQ_GET_INTERFACE:
					io->data[0] = usb_interface.bAlternateSetting;
					io->inner.length = 1;
					return true;
					
				case USB_REQ_SET_INTERFACE:
					printf("No ide what to do");
					return true;
				default:
					printf("fail: USB_TYPE_STANDARD no response\n");
					exit(EXIT_FAILURE);
			}
			break;
		case USB_TYPE_CLASS:
			switch (event->ctrl.bRequest) {
				case HID_REQ_SET_REPORT:
					io->inner.length = event->ctrl.wLength;//1;
//					io->inner.length = 1;
					*done = true;
					return true;
				case HID_REQ_SET_IDLE:
					io->inner.length = 0;
					//*done = true;
					return true;
				case HID_REQ_SET_PROTOCOL:
					io->inner.length = 0;
					*done = true;
					return true;
					
				case HID_REQ_GET_REPORT:
//					printf("fail:  HID_REQ_GET_REPORT e\n", (int)event->ctrl.wLength);
					switch (event->ctrl.wValue & 0xff) {
#ifdef PS4_STRING
						case REPORT_ID_BT_MAC:
							memcpy(&io->data[0], &blueMacAddress, sizeof(blueMacAddress));
							io->inner.length = event->ctrl.wLength;
							return true;
						case REPORT_ID_CALIBRATION:
							memcpy(&io->data[0], &calibrationData, sizeof(calibrationData));
							io->inner.length = event->ctrl.wLength;
							return true;
						case REPORT_ID_HARDWARE_FIRMWARE:
							memcpy(&io->data[0], &hardFirmVersion, sizeof(hardFirmVersion));
							io->inner.length = event->ctrl.wLength;
							*done = true;
							return true;
						case REPORT_NOIDEAWTF:
							memcpy(&io->data[0], &noIdeaWTF, sizeof(noIdeaWTF));
							io->inner.length = event->ctrl.wLength;
							*done = true;
							return true;
#endif
#ifdef PS5_STRING
						case 0x05:
							memcpy(&io->data[0], &unsure05, sizeof(unsure05));
							io->inner.length = event->ctrl.wLength;
							
							return true;
						case 0x20:
							memcpy(&io->data[0], &unsure20, sizeof(unsure20));
							io->inner.length = event->ctrl.wLength;
							
							return true;
						case 0x09:
							memcpy(&io->data[0], &unsure09, sizeof(unsure09));
							io->inner.length = event->ctrl.wLength;
							
							return true;
						case 0x00:
							io->data[0] = 0;
								io->data[1] = 0;
								io->inner.length = event->ctrl.wLength;

							return true;
							
						case 0xf2:
							memcpy(&io->data[0], &unsureF2, sizeof(unsureF2));
								io->inner.length = event->ctrl.wLength;

							return true;
#endif
#ifdef MAGIC_PRO
						case 0x03:
							memcpy(&io->data[0], &magicMagic, sizeof(magicMagic));
							io->inner.length = event->ctrl.wLength;
							//*done = true;
							return true;
#endif
						
						default:
							printf("|||| UNHANDLED HID_REQ_GET_REPORT - event->ctrl.wValue & 0xff == %02x\n", event->ctrl.wValue & 0xff );
							memcpy(&io->data[0], dummyBuffer, event->ctrl.wLength);
							io->data[0] = event->ctrl.wValue & 0xff; // all responses have same header, but this feel hacky
							io->inner.length = event->ctrl.wLength;
							return true;
					}
				case HID_REQ_GET_IDLE:
					printf("fail:  HID_REQ_GET_IDLE\n");
//					io->data[0] = 1;
//					io->inner.length = 1;
					exit(EXIT_FAILURE);	// HACK
					break;
				case HID_REQ_GET_PROTOCOL:
					printf("fail:  HID_REQ_GET_PROTOCOL\n");
					exit(EXIT_FAILURE);	// HACK
					break;
					
				case 0x81:
				case 0x82:
					if (event->ctrl.wIndex == 0x500) {
						io->data[0] = 0;
						io->data[1] = 0;
					} else {
						io->data[0] = 0;
						io->data[1] = 0x9c;
					}
					io->inner.length = event->ctrl.wLength;
					return true;
				case 0x83:
					if (event->ctrl.wIndex == 0x500) {
						io->data[0] = 0;
						io->data[1] = 0x30;
					} else {
						io->data[0] = 0;
						io->data[1] = 0;
					}
					io->inner.length = event->ctrl.wLength;
					return true;
				case 0x84:
					if (event->ctrl.wIndex == 0x500) {
						io->data[0] = 0x7a;
						io->data[1] = 0x00;
					} else {
						io->data[0] = 0;
						io->data[1] = 0x01;
					}
					io->inner.length = event->ctrl.wLength;
					return true;
//				case 0x01:
//				case 0x02:
//
//				case 0x03:
				case 0x04:
//					if (event->ctrl.wIndex == 0x500) {
//						io->data[0] = 0x7a;
//						io->data[1] = 0x00;
//					} else {
//						io->data[0] = 0;
//						io->data[1] = 0x01;
//					}
					io->inner.length = event->ctrl.wLength;
					return true;
				default:
					printf("fail:  USB_TYPE_CLASS no response: event->ctrl.bRequest = %d\n", (int)event->ctrl.bRequest);
					exit(EXIT_FAILURE);	// HACK
					//				*done = true;
					//				return true;
			}
			break;
		case USB_TYPE_VENDOR:
			switch (event->ctrl.bRequest) {
				default:
					printf("fail: USB_TYPE_VENDOR no response\n");
					exit(EXIT_FAILURE);
			}
			break;
		default:
			printf("fail: default: no response\n");
			exit(EXIT_FAILURE);
	}
}

// Not technically a loop as we stop processing ep0 requests to make the
// example simpler.
bool ep0_loop(int fd) {
	bool done = false;
	//while (!done) {
		struct usb_raw_control_event event;
		event.inner.type = 0;
		event.inner.length = sizeof(event.ctrl);

		usb_raw_event_fetch(fd, (struct usb_raw_event *)&event);
		log_event((struct usb_raw_event *)&event);

		if (event.inner.type == USB_RAW_EVENT_CONNECT)
			process_eps_info(fd);

		if (event.inner.type != USB_RAW_EVENT_CONTROL)
			return false;//continue;

		struct usb_raw_control_io io;
		io.inner.ep = 0;
		io.inner.flags = 0;
		io.inner.length = 0;

		bool reply = ep0_request(fd, &event, &io, &done);
		if (!reply) {
			printf("ep0: stalling\n");
			usb_raw_ep0_stall(fd);
			return false;//continue;
		}

		if (event.ctrl.wLength < io.inner.length)
			io.inner.length = event.ctrl.wLength;
		int rv = -1;
		if (event.ctrl.bRequestType & USB_DIR_IN) {
			rv = usb_raw_ep0_write(fd, (struct usb_raw_ep_io *)&io);
			printf("ep0: transferred %d bytes (in) - ", rv);
		} else {
			rv = usb_raw_ep0_read(fd, (struct usb_raw_ep_io *)&io);
			printf("ep0: transferred %d bytes (out) - ", rv);
			
		}
	for (int i = 0; i < io.inner.length; i++) {
		printf("%02x ", io.inner.data[i]);
	}
	printf("\n");
	//}
	
	return done;
}

void* ep0_loop_thread( void* data ) {
	int fd = *(int*)data;
	while(1)
		ep0_loop(fd);
}


void ep_int_in_loop(int fd) {
	//printf(" -- FINALLY in ep_int_in_loop\n");
	struct usb_raw_int_io io;
	io.inner.ep = ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = 64;
	
	
	
	static unsigned int button = 16;
	
	inputReport01_t joy;
	
	

	memset(&joy, 0, sizeof(joy));
	joy.reportId = 0x01;
	
	*((unsigned int*)(8+(unsigned char*)&joy)) |= button;
	
	if (button >= 0x80000000) {
		button = 0x001;
	} else {
		button  = button << 1;
	}
	
	//while (true) {
//		joy.BTN_GamePadButton5 = 1;
//		joy.BTN_GamePadButton1 = 0;
		memcpy(&io.inner.data[0], &joy, sizeof(joy));
		//printf(" -- sending...\n");
		int rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
	if (rv < 0) {
		// error?
	}
		//printf("int_in: key down: %d\n", rv);

//	if(0) {
//	io.inner.ep = ep_int_out;// | 0X04;
//	io.inner.flags = 0;
//	io.inner.length = 32;
//		rv = usb_raw_ep_read(fd, (struct usb_raw_ep_io *)&io);
//	printf("int_out: key down: %d - ", rv);
//	for (int i = 0; i < rv; i++) {
//		printf("%02x ", io.inner.data[i]);
//	}
//	printf("\n");
//	}
//		usleep(50000);
		
//		joy.BTN_GamePadButton5 = 0;
//	joy.BTN_GamePadButton1 = 1;
//		memcpy(&io.inner.data[0], &joy, sizeof(joy));
//		rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
//		printf("int_in: key up: %d\n", rv);

//		usleep(500000);
	//}
}

void ep_int_out_loop(int fd) {
	//printf(" -- FINALLY in ep_int_in_loop\n");
	struct usb_raw_int_io io;
	
	io.inner.ep = ep_int_out;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = 48;//32;
	
	
	

	
	//inputReport01_t joy;

	
		int rv = usb_raw_ep_read(fd, (struct usb_raw_ep_io *)&io);
	printf("int_out: key down: %d - ", rv);
	for (int i = 0; i < rv; i++) {
		printf("%02x ", io.inner.data[i]);
	}
	printf("\n");
	
		
//		joy.BTN_GamePadButton5 = 0;
//	joy.BTN_GamePadButton1 = 1;
//		memcpy(&io.inner.data[0], &joy, sizeof(joy));
//		rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
//		printf("int_in: key up: %d\n", rv);

//		usleep(500000);
	//}
}



void* epin_loop_thread( void* data ) {
	int fd = *(int*)data;
	while(1) {
		if (ep_int_in >= 0) {
			ep_int_in_loop(fd);
			usleep(200000);
		} else {
			usleep(100000);
		}
	}
}

void* epout_loop_thread( void* data ) {
	int fd = *(int*)data;
	while(1) {
		if (ep_int_out >= 0) {
			ep_int_out_loop(fd);
			usleep(1000);
		} else {
			usleep(100000);
		}
	}
}

int main(int argc, char **argv) {
	const char *device = "dummy_udc.0";
	const char *driver = "dummy_udc";
	if (argc >= 2)
		device = argv[1];
	if (argc >= 3)
		driver = argv[2];

	int fd = usb_raw_open();
	usb_raw_init(fd, USB_SPEED_HIGH, driver, device);
	usb_raw_run(fd);

	
	pthread_t threadEp0;
	
	pthread_create(&threadEp0, NULL, ep0_loop_thread, &fd);
	
	pthread_t threadEpOut;
	pthread_t threadEpIn;
	pthread_create(&threadEpIn, NULL, epin_loop_thread, &fd);
	pthread_create(&threadEpOut, NULL, epout_loop_thread, &fd);
	
	//sleep(10);
	//bool allGood = false;
	while(1) {
	//printf(" ---- ep0_loop(fd): start\n");
		sleep(1);
		
		
	//if(ep0_loop(fd))
	//	allGood = true;
		
//	if (allGood) {
//		while(1) {
	//	ep_int_in_loop(fd);
		//printf(" ---- ep_int_in_loop(fd): done\n");
//		}
//	}/
	}
	close(fd);

	return 0;
}

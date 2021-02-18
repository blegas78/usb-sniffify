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
//#include "dualsense.h"
//#include "magicPro.h"

#include <iostream>

#include <libusb.h>


#include "raw-helper.h"

using namespace std;

/*----------------------------------------------------------------------*/

#define VENDOR 0x054c
#define PRODUCT 0x0ce6
/*----------------------------------------------------------------------*/

//struct usb_endpoint_descriptor usb_endpoints[4] = {
//	{
//		.bLength =		USB_DT_ENDPOINT_SIZE,
//		.bDescriptorType =	USB_DT_ENDPOINT,
//		.bEndpointAddress =	USB_DIR_IN | 0X04,
//		.bmAttributes =		USB_ENDPOINT_XFER_INT,
//		.wMaxPacketSize =	EP_MAX_PACKET_INT,
//		.bInterval =		5,
//	}, {
//		.bLength =		USB_DT_ENDPOINT_SIZE,
//		.bDescriptorType =	USB_DT_ENDPOINT,
//		.bEndpointAddress =	USB_DIR_OUT | 0x03,
//		.bmAttributes =		USB_ENDPOINT_XFER_INT,
//		.wMaxPacketSize =	EP_MAX_PACKET_INT,
//		.bInterval =		5,
//	}, {
//		.bLength =		9,
//		.bDescriptorType =	5,
//		.bEndpointAddress =	USB_DIR_IN | 0X02,
//		.bmAttributes =		5,
//		.wMaxPacketSize =	0x00c4,
//		.bInterval =		4,
//	}, {
//		.bLength =		9,
//		.bDescriptorType =	5,
//		.bEndpointAddress =	USB_DIR_OUT | 0x01,
//		.bmAttributes =		9,
//		.wMaxPacketSize =	0x0188,
//		.bInterval =		4,
//	}
//};
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



typedef struct {
	int fd = -1;	// raw_gadget descriptor
	int ep_int = -1;// endpoint handler
	libusb_device_handle *deviceHandle;
	bool keepRunning = true;	// thread management
	
	struct usb_endpoint_descriptor usb_endpoint;
	uint8_t bmAttributes;
	uint8_t bEndpointAddress;
	uint16_t wMaxPacketSize;
	
	unsigned char* data;
} EndpointInfo;

typedef struct {
	int totalEndpoints;
	EndpointInfo *mEndpointInfos;
	int fd;
	libusb_device_handle *dev_handle;
} EndpointZeroInfo;





void process_eps_info(EndpointZeroInfo* epZeroInfo) {
	struct usb_raw_eps_info info;
	memset(&info, 0, sizeof(info));
	
	int num = usb_raw_eps_info(epZeroInfo->fd, &info);
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
	
	for (int e = 0; e < epZeroInfo->totalEndpoints; e++) {
		for (int i = 0; i < num; i++) {
//			if (assign_ep_address(&info.eps[i], &usb_endpoints[e]))
			if (assign_ep_address(&info.eps[i], &epZeroInfo->mEndpointInfos[e].usb_endpoint))
				continue;
		}
		
//		int int_in_addr = usb_endpoint_num(&usb_endpoints[e]);
		int int_in_addr = usb_endpoint_num(&epZeroInfo->mEndpointInfos[e].usb_endpoint);
		assert(int_in_addr != 0);
		printf("int_in: addr = %u\n", int_in_addr);
	}
}



//libusb_device_handle *dev_handle;
int len;

int ep_int_in = -1;
int ep_int_out = -1;
int ep_int_in_audio = -1;
int ep_int_out_audio = -1;
char dummyBuffer[128];
bool ep0_request(EndpointZeroInfo* info, struct usb_raw_control_event *event,
				 struct usb_raw_control_io *io, bool *done) {
	int r;
	

	
//	if (event->ctrl.wIndex == 0x02 ||
//		event->ctrl.wIndex == 0x03 ||
//		event->ctrl.wIndex == 0x01 ) {
	//		event->ctrl.wIndex = 0x00;
	//	}
	io->inner.length = event->ctrl.wLength;
	if (event->ctrl.bRequestType & 0x80) {
		std::cout << "copying " << (int)event->ctrl.wLength << " bytes" << std::endl;
		//		memcpy(dummyBuffer, &event->inner.data[0], event->ctrl.wLength);
		
		r = libusb_control_transfer(	info->dev_handle,
									event->ctrl.bRequestType,
									event->ctrl.bRequest,
									event->ctrl.wValue,
									event->ctrl.wIndex,
									//								event->inner.data,
//									dummyBuffer,
									(unsigned char*)&io->data[0],
									event->ctrl.wLength,
									0);
		if (r < 0) {
			std::cerr << "r<0 :(" << std::endl;
			printf("libusb_control_transfer error: %s\n", libusb_error_name(r));
			return false;
		}
		
		//	memcpy(&io->inner.data[0], dummyBuffer, r);
//		memcpy(&io->data[0], dummyBuffer, r);
		//	memcpy(&io->inner.data[0], event->inner.data, r);
		io->inner.length = r;
	}
	
	if( (event->ctrl.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD &&
	   event->ctrl.bRequest == USB_REQ_SET_CONFIGURATION) {
		
//			if (ep_int_in  < 0) {
//				ep_int_in = usb_raw_ep_enable(fd, &usb_endpoints[0]);
//				ep_int_in_audio = usb_raw_ep_enable(fd, &usb_endpoints[2]);
//			}
//			if (ep_int_out < 0) {
//				ep_int_out = usb_raw_ep_enable(fd, &usb_endpoints[1]);
//				ep_int_out_audio = usb_raw_ep_enable(fd, &usb_endpoints[3]);
//			}
//
//			printf(" ---- ep_int_in = %d\n", ep_int_in);
//			printf(" ---- ep_int_out = %d\n", ep_int_out);
//			printf(" ---- ep_int_in_audio = %d\n", ep_int_in_audio);
//		   printf(" ---- ep_int_out_audio = %d\n", ep_int_out_audio);
		
		for (int i = 0; i < info->totalEndpoints; i++) {
			info->mEndpointInfos[i].ep_int = usb_raw_ep_enable(info->fd, &info->mEndpointInfos[i].usb_endpoint);
			printf(" ---- 0x%02x ep_int = %d\n", info->mEndpointInfos[i].bEndpointAddress, info->mEndpointInfos[i].ep_int);
		}
		
			usb_raw_vbus_draw(info->fd, 0x32*5);
			usb_raw_configure(info->fd);
//			io->inner.length = 0;
	}
	return true;
}


// Not technically a loop as we stop processing ep0 requests to make the
// example simpler.
bool ep0_loop(EndpointZeroInfo* info) {
	bool done = false;
	//while (!done) {
	struct usb_raw_control_event event;
	event.inner.type = 0;
	event.inner.length = sizeof(event.ctrl);
	
	usb_raw_event_fetch(info->fd, (struct usb_raw_event *)&event);
	log_event((struct usb_raw_event *)&event);
	
	if (event.inner.type == USB_RAW_EVENT_CONNECT)
		process_eps_info(info);
	
	if (event.inner.type != USB_RAW_EVENT_CONTROL)
		return false;//continue;
	
	struct usb_raw_control_io io;
	io.inner.ep = 0;
	io.inner.flags = 0;
	io.inner.length = 0;
	
	bool reply = ep0_request(info, &event, &io, &done);
	if (!reply) {
		printf("ep0: stalling\n");
		usb_raw_ep0_stall(info->fd);
		return false;//continue;
	}
	//printf("ep0: ep0_request() Success!\n");
	
	if (event.ctrl.wLength < io.inner.length)
		io.inner.length = event.ctrl.wLength;
	int rv = -1;
	if (event.ctrl.bRequestType & USB_DIR_IN) {
		rv = usb_raw_ep0_write(info->fd, (struct usb_raw_ep_io *)&io);
		printf("ep0: transferred %d bytes (in) - ", rv);
	} else {
		rv = usb_raw_ep0_read(info->fd, (struct usb_raw_ep_io *)&io);
		printf("ep0: transferred %d bytes (out) - ", rv);
		

//			std::cout << "copying " << (int)event->ctrl.wLength << " bytes" << std::endl;

		int r = libusb_control_transfer(	info->dev_handle,
									event.ctrl.bRequestType,
									event.ctrl.bRequest,
									event.ctrl.wValue,
									event.ctrl.wIndex,
	//								event->inner.data,
									(unsigned char*)&io.data[0],
//									dummyBuffer,
									io.inner.length,
//									event->ctrl.wLength,
									5);
		
		if (r < 0) {
			printf(" ERROR  libusb_control_transfer() returned < 0 in ep0_loop(). r = %d\n", r);
		}
	}
	for (__u32 i = 0; i < io.inner.length; i++) {
		printf("%02x ", io.inner.data[i]);
	}
	printf("\n");
	//}
	
	return done;
}

void* ep0_loop_thread( void* data ) {
//	int fd = *(int*)data;
	EndpointZeroInfo* info = (EndpointZeroInfo *)data;
	while(1)
		ep0_loop(info);//fd);
}
/*
void ep_int_in_audio_loop(int fd) {
	struct usb_raw_int_io io;
	io.inner.ep = ep_int_in_audio;// | 0X82;
	io.inner.flags = 0;
	io.inner.length = 0x00c4;
	
	int rv = usb_raw_ep_write(fd, (struct usb_raw_ep_io *)&io);
	if (rv < 0) {
		// ? error
	}
}

void ep_int_out_audio_loop(int fd) {
	struct usb_raw_int_io io;
	io.inner.ep = ep_int_out_audio;// | 0X82;
	io.inner.flags = 0;
	io.inner.length = 0x0188;
	
	int rv = usb_raw_ep_read(fd, (struct usb_raw_ep_io *)&io);
	if (rv < 0) {
		// ? error
	}
}

void ep_int_in_loop(int fd) {
	//printf(" -- FINALLY in ep_int_in_loop\n");
	struct usb_raw_int_io io;
	io.inner.ep = ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = 64;
	
	
	
//	static unsigned int button = 16;
//
//	inputReport01_t joy;
//
//
//
//	memset(&joy, 0, sizeof(joy));
//	joy.reportId = 0x01;
//
//	*((unsigned int*)(8+(unsigned char*)&joy)) |= button;
//
//	if (button >= 0x80000000) {
	//		button = 0x001;
	//	} else {
	//		button  = button << 1;
	//	}
	
	//while (true) {
	//		joy.BTN_GamePadButton5 = 1;
	//		joy.BTN_GamePadButton1 = 0;
	//	memcpy(&io.inner.data[0], &joy, sizeof(joy));
	//printf(" -- sending...\n");
	unsigned char data[64];
	int transferred = 64;
	int r = libusb_interrupt_transfer( dev_handle,
									  0x84,
									  data,
									  64,
									  &transferred,
									  0 );
	if (r != 0) {
	//	printf("libusb_interrupt_transfer FAIL\n");
		return;
	}
//	printf("Read %d from controller\n", transferred);
	memcpy(&io.inner.data[0], &data, transferred);
	//io.inner.length = transferred;
	
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
	
	unsigned char data[48];
	int transferred = rv;
	
	memcpy(&data, &io.inner.data[0], transferred);
	
	int r = libusb_interrupt_transfer( dev_handle,
									  0x03,
									  data,
									  rv,
									  &transferred,
									  0 );
	if (r != 0) {
	//	printf("libusb_interrupt_transfer FAIL\n");
		return;
	}
}



void* epin_loop_thread( void* data ) {
	int fd = *(int*)data;
	while(1) {
		if (ep_int_in >= 0) {
			ep_int_in_loop(fd);
			//usleep(1000);
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
//			usleep(100);
		} else {
			usleep(100000);
		}
	}
}

void* epin_loop_thread_audio( void* data ) {
	int fd = *(int*)data;
	while(1) {
		if (ep_int_in_audio >= 0) {
			ep_int_in_audio_loop(fd);
			//usleep(1000);
		} else {
			usleep(100000);
		}
	}
}

void* epout_loop_thread_audio( void* data ) {
	int fd = *(int*)data;
	while(1) {
		if (ep_int_out_audio >= 0) {
			ep_int_out_audio_loop(fd);
//			usleep(100);
		} else {
			usleep(100000);
		}
	}
}
*/


void ep_out_work_interrupt( EndpointInfo* epInfo ) {
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;//ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = epInfo->wMaxPacketSize;
	
	int rv = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
	printf("ep_out_work_interrupt(): %d - ", rv);
	for (int i = 0; i < rv; i++) {
		printf("%02x ", io.inner.data[i]);
	}
	printf("\n");
	
	//unsigned char data[48];
	int transferred = rv;
	
	memcpy(epInfo->data, &io.inner.data[0], transferred);
	
	int r = libusb_interrupt_transfer(epInfo->deviceHandle,
									  epInfo->bEndpointAddress,
									  epInfo->data,
									  rv,
									  &transferred,
									  0 );
	if (r != 0) {
	//	printf("libusb_interrupt_transfer FAIL\n");
		return;
	}
}
	
void ep_in_work_interrupt( EndpointInfo* epInfo ) {
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;//ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = epInfo->wMaxPacketSize;
	
	int transferred = epInfo->wMaxPacketSize;
	int r = libusb_interrupt_transfer(epInfo->deviceHandle,
									  epInfo->bEndpointAddress,
									  epInfo->data,
									  epInfo->wMaxPacketSize,
									  &transferred,
									  0 );
	if (r != 0) {
	//	printf("libusb_interrupt_transfer FAIL\n");
		return;
	}
//	printf("Read %d from controller\n", transferred);
	memcpy(&io.inner.data[0], epInfo->data, transferred);
	//io.inner.length = transferred;
	
	int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
	if (rv < 0) {
		// error?
	}
}

int yesImDone = true;
static void cb_transfer_iso_in(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		for (int i = 0; i < xfr->num_iso_packets; i++) {
			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
				return;
			}
			
//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
			
			
			
			EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
			struct usb_raw_int_io io;
			io.inner.ep = epInfo->ep_int;
			io.inner.flags = 0;
			io.inner.length = pack->actual_length;//0;//epInfo->wMaxPacketSize;

			// TODO: everything, really

//			memcpy(&io.inner.data[0], epInfo->data, pack->actual_length);
			memcpy(&io.inner.data[0], xfr->buffer, pack->actual_length);

//			int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
//			if (rv < 0) {
//				// ? error
//			}
		}
	}
	yesImDone = true;
}

void ep_in_work_isochronous( EndpointInfo* epInfo ) {
	if (!yesImDone) {
		usleep(1);
		return;
	}
	static struct libusb_transfer *xfr;
	int num_iso_pack = 1;
	xfr = libusb_alloc_transfer(num_iso_pack);
	char buffer[200];
	libusb_fill_iso_transfer(xfr,
							 epInfo->deviceHandle,
							 epInfo->bEndpointAddress,
							 buffer,//epInfo->data,
							 epInfo->wMaxPacketSize,
							 num_iso_pack,
							 cb_transfer_iso_in,
							 epInfo,
							 0);
	libusb_set_iso_packet_lengths(xfr, epInfo->wMaxPacketSize/num_iso_pack);

	libusb_submit_transfer(xfr);
	yesImDone = false;
	
	usleep(10000);
}
static void cb_xfr(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
	}
	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		for (int i = 0; i < xfr->num_iso_packets; i++) {
			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
				//exit(5);
			}
			
//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
		}
	}
//	printf("xfr Success!\n");
}
void ep_out_work_isochronous( EndpointInfo* epInfo ) {
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;
	io.inner.flags = 0;
	io.inner.length = epInfo->wMaxPacketSize;
	
	int rv = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
	if (rv < 0) {
		printf("No data vailable I guess?\n");
		return;
	}
//	printf("ep_out_work_isochronous(): %d - ", rv);
//	for (int i = 0; i < rv; i++) {
//		printf("%02x ", io.inner.data[i]);
//	}
//	printf("\n");
	int transferred = rv;
	memcpy(epInfo->data, &io.inner.data[0], transferred);
	
	static struct libusb_transfer *xfr;
	int num_iso_pack = 1;
	xfr = libusb_alloc_transfer(num_iso_pack);
	libusb_fill_iso_transfer(xfr,
							 epInfo->deviceHandle,
							 epInfo->bEndpointAddress,
							 epInfo->data,
							 rv,
							 num_iso_pack,
							 cb_xfr,
							 NULL,
							 0);
	libusb_set_iso_packet_lengths(xfr, rv/num_iso_pack);
	
	libusb_submit_transfer(xfr);
	
	if (rv < 0) {
		// ? error
	}
}

void* ep_loop_thread( void* data ) {
	EndpointInfo *ep = (EndpointInfo*)data;
	while(ep->keepRunning) {
		if (ep->ep_int >= 0) {
			if (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) {	// data in
//				ep_int_in_loop(ep->fd);
				switch (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_INTERRUPT:
						ep_in_work_interrupt( ep ); break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
						ep_in_work_isochronous( ep ); break;
					case LIBUSB_TRANSFER_TYPE_CONTROL:
					case LIBUSB_TRANSFER_TYPE_BULK:
					default:
						std::cout << "Unsupported ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK" << std::endl;
						usleep(1000);
						break;
				}
			} else { // data out
				switch (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_INTERRUPT:
						ep_out_work_interrupt( ep ); break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
						ep_out_work_isochronous( ep ); break;
					case LIBUSB_TRANSFER_TYPE_CONTROL:
					case LIBUSB_TRANSFER_TYPE_BULK:
					default:
						std::cout << "Unsupported ep->bEndpointAddress" << std::endl;
						usleep(1000);
						break;
				}
			}
		} else {
			printf("Idle: Endpoint 0x%02x\n", ep->bEndpointAddress);
			usleep(1000000);
		}
	}
	return NULL;
}


int main(int argc, char **argv) {
	const char *device = "dummy_udc.0";
	const char *driver = "dummy_udc";
	if (argc >= 2)
		device = argv[1];
	if (argc >= 3)
		driver = argv[2];
	
	int fd = usb_raw_open();
	
	
	
	
	libusb_device **devices;
	libusb_device_handle *deviceHandle;
	libusb_context *context = NULL;
	
	int r = libusb_init(&context);
	if(r < 0) {
		std::cout << "libusb_init() Error " << r << std::endl;
		return 1;
	}
	//libusb_set_debug(context, 0);
	libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

	ssize_t deviceCount = libusb_get_device_list(context, &devices);
	if(deviceCount < 0) {
		std::cout << "libusb_get_device_list() Error" << std::endl;
		return 1;
	}
	std::cout << deviceCount << " Devices." << std::endl;
	
	int devIndex = -1;
	for (int i = 0; i < deviceCount; i++) {
		std::cout << "Device : " << i;
		std::cout << " | Bus " << (int)libusb_get_bus_number(devices[i]);
		std::cout << " Port " << (int)libusb_get_port_number(devices[i]) << std::endl;
//		std::cout << " - Addr " << (int)libusb_get_device_address(devices[i]) << std::endl;
		
		if (libusb_get_bus_number(devices[i]) == 1 &&	// specific for lower USB2 port on rPi 4 with raspbian
			libusb_get_port_number(devices[i]) == 4) {
			devIndex = i;
			std::cout << " |-- This is the device of interest!" << std::endl;
		}
	}
	if (devIndex < 0) {
		std::cerr << "ERROR!  No USB on the lower rPi USB 2.0 port?" << std::endl;
		exit(EXIT_FAILURE);
	}
	if( libusb_open(devices[devIndex], &deviceHandle)  != LIBUSB_SUCCESS ) {
		std::cerr << "FAILED! libusb_open()" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Device has been opened!" << std::endl;
//	deviceHandle = libusb_open_device_with_vid_pid(context, VENDOR, PRODUCT); //these are vendorID and productID I found for my usb device
//	if(deviceHandle == NULL)
//		cout<<"Cannot open device"<<endl;
//	else
//		cout<<"Device Opened"<<endl;
	libusb_free_device_list(devices, 1); //free the list, unref the devices in it

	
	
	if(libusb_kernel_driver_active(deviceHandle, 0) == 1) { //find out if kernel driver is attached
		std::cout << "Kernel Driver Active" << std::endl;
		if(libusb_detach_kernel_driver(deviceHandle, 0) == 0) //detach it
			std::cout << "Kernel Driver Detached!" << std::endl;
	}
	if (libusb_set_auto_detach_kernel_driver(deviceHandle, true) != LIBUSB_SUCCESS) {
		std::cerr << "FAILED to perform libusb_set_auto_detach_kernel_driver()" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	libusb_device_descriptor deviceDescriptor;
	if( libusb_get_device_descriptor(libusb_get_device(deviceHandle), &deviceDescriptor) != LIBUSB_SUCCESS ) {
		std::cerr << "FAILED to call libusb_get_device_descriptor()" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Have Device Descriptor!" << std::endl;
	std::cout << " - bLength            : " << (int)deviceDescriptor.bLength << std::endl;
	std::cout << " - bDescriptorType    : " << (int)deviceDescriptor.bDescriptorType << std::endl;
	printf(      " - bcdUSB             : 0x%04x\n", deviceDescriptor.bcdUSB);
	std::cout << " - bDeviceClass       : " << (int)deviceDescriptor.bDeviceClass << std::endl;
	std::cout << " - bDeviceSubClass    : " << (int)deviceDescriptor.bDeviceSubClass << std::endl;
	std::cout << " - bDeviceProtocol    : " << (int)deviceDescriptor.bDeviceProtocol << std::endl;
	std::cout << " - bMaxPacketSize0    : " << (int)deviceDescriptor.bMaxPacketSize0 << std::endl;
	printf(      " - idVendor           : 0x%04x\n", deviceDescriptor.idVendor);
	printf(      " - idProduct          : 0x%04x\n", deviceDescriptor.idProduct);
	std::cout << " - bcdDevice          : " << (int)deviceDescriptor.bcdDevice << std::endl;
	std::cout << " - iManufacturer      : " << (int)deviceDescriptor.iManufacturer << std::endl;
	std::cout << " - iProduct           : " << (int)deviceDescriptor.iProduct << std::endl;
	std::cout << " - iSerialNumber      : " << (int)deviceDescriptor.iSerialNumber << std::endl;
	std::cout << " - bNumConfigurations : " << (int)deviceDescriptor.bNumConfigurations << std::endl;
	
	if (deviceDescriptor.bNumConfigurations != 1) {
		std::cerr << "ERROR!  No support for multiple configurations, deviceDescriptor.bNumConfigurations = " << (int)deviceDescriptor.bNumConfigurations << std::endl;
	}
	
	int configIndex = 0;
	libusb_config_descriptor* configDescriptor;
	
	if(libusb_get_config_descriptor(libusb_get_device(deviceHandle), configIndex, &configDescriptor) != LIBUSB_SUCCESS) {
		std::cerr << "FAILED! libusb_get_config_descriptor()" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Have Config Descriptor!" << std::endl;
	std::cout << " - bLength            : " << (int)configDescriptor->bLength << std::endl;
	std::cout << " - bDescriptorType    : " << (int)configDescriptor->bDescriptorType << std::endl;
	std::cout << " - wTotalLength       : " << (int)configDescriptor->wTotalLength << std::endl;
	std::cout << " - bNumInterfaces     : " << (int)configDescriptor->bNumInterfaces << std::endl;
	std::cout << " - bConfigurationValue: " << (int)configDescriptor->bConfigurationValue << std::endl;
	std::cout << " - iConfiguration     : " << (int)configDescriptor->iConfiguration << std::endl;
	std::cout << " - bmAttributes       : " << (int)configDescriptor->bmAttributes << std::endl;
	std::cout << " - MaxPower           : " << (int)configDescriptor->MaxPower << std::endl;
	std::cout << " - extra_length       : " << (int)configDescriptor->extra_length << std::endl;
	
	
	int numInterfaces = configDescriptor->bNumInterfaces;
	int totalEndpoints = 0;
	for (int i = 0; i < numInterfaces; i++) {
		int numAlternates = configDescriptor->interface[i].num_altsetting;
		
		for (int a = 0; a < numAlternates; a++) {
			const libusb_interface_descriptor *interfaceDescriptor = &configDescriptor->interface[i].altsetting[a];
			std::cout << " | - Interface " << (int)interfaceDescriptor->bInterfaceNumber << " Alternate " << a << std::endl;
			
			r = libusb_claim_interface(deviceHandle, interfaceDescriptor->bInterfaceNumber); //claim interface 0 (the first) of device (mine had jsut 1)
			if(r < 0) {
				cout << "Cannot Claim Interface" << endl;
				return 1;
			}
			//		cout << "Claimed Interface " << (int)configDescriptor->interfaces[i].altsetting->bInterfaceNumber <<endl;
			
			totalEndpoints += interfaceDescriptor->bNumEndpoints;
			std::cout << "   - bNumEndpoints      : " << (int)interfaceDescriptor->bNumEndpoints << std::endl;
			std::cout << "   - Endpoints          : " <<std::endl;
			for (int e = 0; e < interfaceDescriptor->bNumEndpoints; e++) {
				libusb_set_interface_alt_setting(deviceHandle, i, a );	// no idea how to use this properly, but putting htis here wrok son a PS5 controller
				
				const libusb_endpoint_descriptor *endpointDescriptor = &interfaceDescriptor->endpoint[e];
				printf("   | - bEndpointAddress   : 0x%02x\n", endpointDescriptor->bEndpointAddress);
				printf("     - wMaxPacketSize     : %d\n", endpointDescriptor->wMaxPacketSize);
				printf("     - bmAttributes       : %d\n", endpointDescriptor->bmAttributes);
				
				switch (endpointDescriptor->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_CONTROL: printf("     | - Control\n"); break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS: printf("     | - Isochronous\n"); break;
					case LIBUSB_TRANSFER_TYPE_BULK: printf("     | - Bulk\n"); break;
					case LIBUSB_TRANSFER_TYPE_INTERRUPT: printf("     | - Interrupt\n"); break;
					default:
						break;
				}
				
			}
		}
	}
	
	
	
	// raw-gadget fun
	std::cout << "Starting raw-gadget" << std::endl;
	usb_raw_init(fd, USB_SPEED_HIGH, driver, device);
	usb_raw_run(fd);

	
	// Build endpoint handling
	std::cout << "Starting endpoint threads" << std::endl;
	EndpointInfo *mEndpointInfos = (EndpointInfo *)malloc(totalEndpoints * sizeof(EndpointInfo));
	pthread_t *endpointThreads = (pthread_t *)malloc(totalEndpoints * sizeof(pthread_t));
	
	int endpoint = 0;
	for (int i = 0; i < numInterfaces; i++) {
		int numAlternates = configDescriptor->interface[i].num_altsetting;
		for (int a = 0; a < numAlternates; a++) {
			const libusb_interface_descriptor *interfaceDescriptor = &configDescriptor->interface[i].altsetting[a];
			for (int e = 0; e < interfaceDescriptor->bNumEndpoints; e++) {
				const libusb_endpoint_descriptor *endpointDescriptor = &interfaceDescriptor->endpoint[e];
				mEndpointInfos[endpoint].bmAttributes = endpointDescriptor->bmAttributes;
				mEndpointInfos[endpoint].wMaxPacketSize = endpointDescriptor->wMaxPacketSize;
				mEndpointInfos[endpoint].bEndpointAddress = endpointDescriptor->bEndpointAddress;
				mEndpointInfos[endpoint].data = (unsigned char*)malloc( endpointDescriptor->wMaxPacketSize * sizeof(unsigned char));
				
				mEndpointInfos[endpoint].usb_endpoint.bLength =	endpointDescriptor->bLength;
				mEndpointInfos[endpoint].usb_endpoint.bDescriptorType =	endpointDescriptor->bDescriptorType;
				mEndpointInfos[endpoint].usb_endpoint.bEndpointAddress = endpointDescriptor->bEndpointAddress;
				mEndpointInfos[endpoint].usb_endpoint.bmAttributes = endpointDescriptor->bmAttributes;
				mEndpointInfos[endpoint].usb_endpoint.wMaxPacketSize = endpointDescriptor->wMaxPacketSize;
				mEndpointInfos[endpoint].usb_endpoint.bInterval = endpointDescriptor->bInterval;
				
				mEndpointInfos[endpoint].deviceHandle = deviceHandle;
				mEndpointInfos[endpoint].ep_int = -1;
				mEndpointInfos[endpoint].keepRunning = true;
				
				mEndpointInfos[endpoint].fd = fd;
				
				pthread_create(&endpointThreads[endpoint], NULL, ep_loop_thread, &mEndpointInfos[endpoint]);
				
				endpoint++;
			}
		}
	}
	
	
	
	// Start ep0 thread afer endpoints, I believe
	std::cout << "Starting ep0 thread" << std::endl;
	EndpointZeroInfo mEndpointZeroInfo;
	mEndpointZeroInfo.mEndpointInfos = mEndpointInfos;
	mEndpointZeroInfo.fd = fd;
	mEndpointZeroInfo.dev_handle = deviceHandle;
	mEndpointZeroInfo.totalEndpoints = totalEndpoints;
//	dev_handle = deviceHandle;
	pthread_t threadEp0;
	pthread_create(&threadEp0, NULL, ep0_loop_thread, &mEndpointZeroInfo);//fd);
	
	
//	std::cout << "Starting legacy ep threads" << std::endl;
//	pthread_t threadEpOut;
//	pthread_t threadEpIn;
//	pthread_create(&threadEpIn, NULL, epin_loop_thread, &fd);
//	pthread_create(&threadEpOut, NULL, epout_loop_thread, &fd);
//
//	pthread_t threadEpOut_audio;
//	pthread_t threadEpIn_audio;
//	pthread_create(&threadEpIn_audio, NULL, epin_loop_thread_audio, &fd);
//	pthread_create(&threadEpOut_audio, NULL, epout_loop_thread_audio, &fd);

	
//	pthread_t threadEp[USB_RAW_EPS_NUM_MAX];
//	for (int i = 0; i < USB_RAW_EPS_NUM_MAX; i++) {
//		pthread_create(&threadEp[i], NULL, epin_loop_thread, &mEndpointInfo[i]);
//	}
	
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1;
	while(1) {
//		sleep(1);
		if(libusb_handle_events_timeout(context, &timeout) != LIBUSB_SUCCESS) {	// needed for iso transfers I believe
			printf("libusb_handle_events() FAILED\n");
		}
	}
	close(fd);
	
	for (int i = 0; i < totalEndpoints; i++) {
		free(mEndpointInfos[i].data);
	}
	free(mEndpointInfos);
	return 0;
}

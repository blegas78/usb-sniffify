// SPDX-License-Identifier: GPL-2.0-only

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <math.h>

#include <linux/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>

#include <libusb.h>

#include "raw-helper.h"

/*----------------------------------------------------------------------*/

void setEndpoint(AlternateInfo* info, int endpoint, bool enable) {
	EndpointInfo* endpointInfo = &info->mEndpointInfos[endpoint];
	if (enable) {
		printf(" - - - Attempting to enable EP\n");
		endpointInfo->ep_int = usb_raw_ep_enable(endpointInfo->fd, &endpointInfo->usb_endpoint);
		//libusb_set_interface_alt_setting
		endpointInfo->stop = false;
	} else {	// may need mutex here
		int temp = endpointInfo->ep_int;
		endpointInfo->stop = true;
		while(endpointInfo->busyPackets > 0) {
			usleep(100);
		}
		printf(" - - - Attempting to disable EP with: %d\n", temp);
		int ret = usb_raw_ep_disable(endpointInfo->fd, temp);
		printf(" - - - No idea abbout this ep diable return: %d\n", ret);
		
	}
	printf(" ---- 0x%02x ep_int = %d\n", endpointInfo->usb_endpoint.bEndpointAddress, endpointInfo->ep_int);
}

void setAlternate(InterfaceInfo* info, int alternate) {
	AlternateInfo* alternateInfo = &info->mAlternateInfos[alternate];
	if (alternate >= 0) {
		alternateInfo = &info->mAlternateInfos[alternate];
	} else {
		alternateInfo = &info->mAlternateInfos[info->activeAlternate];
	}
	
	
	if (info->activeAlternate != alternate &&
		info->activeAlternate >= 0 &&
		alternate >= 0) {
		printf(" - - Need to disable current Alternate %d!\n", info->activeAlternate);	// TODO;
		for (int i = 0; i < info->mAlternateInfos[info->activeAlternate].bNumEndpoints; i++) {
			printf(" - - | setEndpoint(?, %d, %d)\n", i, false);
			setEndpoint(&info->mAlternateInfos[info->activeAlternate], i, false);
		}
	}
	for (int i = 0; i < alternateInfo->bNumEndpoints; i++) {
		printf(" - - setEndpoint(?, %d, %d)\n", i, alternate >= 0 ? true : false);
		setEndpoint(alternateInfo, i, alternate >= 0 ? true : false);
	}
	
	
	
	info->activeAlternate = alternate;
}

void setInterface(libusb_device_handle *deviceHandle, ConfigurationInfo* info, int interface, int alternate) {
	InterfaceInfo* interfaceInfo = &info->mInterfaceInfos[interface];
	
	if (info->activeInterface != interface &&
		info->activeInterface >= 0 &&
		alternate > 0) {
		//printf(" - Need to disable current Interface of %d,%d!\n", info->activeInterface, info->mInterfaceInfos[info->activeInterface].activeAlternate);
		//setAlternate(&info->mInterfaceInfos[info->activeInterface], -1);
	}
	
	printf(" - setAlternate(?, %d)\n", alternate);
	setAlternate(interfaceInfo, alternate);
	info->activeInterface = interface;
	if (alternate >= 0) {
		if(libusb_set_interface_alt_setting(deviceHandle, interface, alternate ) != LIBUSB_SUCCESS)  {
			printf("LIBUSB_ERROR! libusb_set_interface_alt_setting()");
		}
	}
	
}

void setConfiguration(EndpointZeroInfo* info, int configuration) {
	ConfigurationInfo* configInfo = &info->mConfigurationInfos[configuration];

	
	if (info->activeConfiguration != configuration &&
		info->activeConfiguration >= 0 &&
		configuration >= 0) {
		printf("Need to disable current configuration!");
		for (int i = 0; i < info->mConfigurationInfos[info->activeConfiguration].bNumInterfaces; i++) {
			setInterface(info->dev_handle, &info->mConfigurationInfos[info->activeConfiguration], i, -1);	// unsure if this is needed in set config
		}
	}
	
	for (int i = 0; i < configInfo->bNumInterfaces; i++) {
		printf("setInterface(?, %d, %d)\n", i, 0);
		setInterface(info->dev_handle, configInfo, i, 0);	// unsure if this is needed in set config
	}
	info->activeConfiguration = configuration;
}


char dummyBuffer[4096];
bool ep0_request(EndpointZeroInfo* info, struct usb_raw_control_event *event,
				 struct usb_raw_control_io *io, bool *done) {
	int r;
	
	io->inner.length = event->ctrl.wLength;
	if (event->ctrl.bRequestType & 0x80) {
		printf("copying %d bytes\n", event->ctrl.wLength);
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
			printf("libusb_control_transfer error: %s\n", libusb_error_name(r));
			return false;
		}
		
		//	memcpy(&io->inner.data[0], dummyBuffer, r);
		//		memcpy(&io->data[0], dummyBuffer, r);
		//	memcpy(&io->inner.data[0], event->inner.data, r);
		io->inner.length = r;
	}
	
	if( (event->ctrl.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch(event->ctrl.bRequest) {
			case USB_REQ_SET_CONFIGURATION:
				printf(" - Setting Configuration to: %d\n",  event->ctrl.wValue & 0xff);	// "The lower byte of the wValue field specifies the desired configuration"
				
//				for (int i = 0; i < info->totalEndpoints; i++) {
//					info->mEndpointInfos[i].ep_int = usb_raw_ep_enable(info->fd, &info->mEndpointInfos[i].usb_endpoint);
//					printf(" ---- 0x%02x ep_int = %d\n", info->mEndpointInfos[i].usb_endpoint.bEndpointAddress, info->mEndpointInfos[i].ep_int);
//				}
				// from: https://usb.ktemkin.com/files/8a35f1350a32d1b2ab2efa5c3e5036e30b9cdcac53a4f772a8db122fcbe2a1a1/usb_device_framework_chapter.pdf
				//8. Based on the configuration information and how the USB device will be used, the host assigns a configuration value to the device. The device is now in the Configured state and all of the endpoints in this configuration have taken on their described characteristics. The USB device may now draw the amount of VBUS power described in its descriptor for the selected configuration. From the device’s point of view, it is now ready for use.
				setConfiguration(info, (event->ctrl.wValue & 0xff) -1);
				
				usb_raw_vbus_draw(info->fd, 0x32*5); // TODO: grab from descriptor for passthrough
				usb_raw_configure(info->fd);
				//			io->inner.length = 0;
				break;
			case USB_REQ_SET_INTERFACE:
				printf(" - Setting Interface to: %d Alternate: %d\n", event->ctrl.wIndex,  event->ctrl.wValue);
				setInterface(info->dev_handle, &info->mConfigurationInfos[info->activeConfiguration], event->ctrl.wIndex,  event->ctrl.wValue);
				break;
			default:
				break;
		}
	}
	return true;
}


bool ep0_loop(EndpointZeroInfo* info) {
	bool done = false;
	//while (!done) {
	struct usb_raw_control_event event;
	event.inner.type = 0;
	event.inner.length = sizeof(event.ctrl);
	
	usb_raw_event_fetch(info->fd, (struct usb_raw_event *)&event);
	log_event((struct usb_raw_event *)&event);
	
	switch (event.inner.type) {
		case USB_RAW_EVENT_CONNECT:
			printf("ep0_loop(): Recieved a USB_RAW_EVENT_CONNECT\n");
			process_eps_info(info);
			return false;
			break;
			
		case USB_RAW_EVENT_CONTROL:
			break;	// continue for processing
			
		default:
			printf("ep0_loop(): event.inner.type != USB_RAW_EVENT_CONTROL, event.inner.type = %d\n", event.inner.type);
			return false;
			break;
	}
//	if (event.inner.type == USB_RAW_EVENT_CONNECT) {
//		printf("ep0_loop(): Recieved a USB_RAW_EVENT_CONNECT\n");
//		process_eps_info(info);
//	}
//
//	if (event.inner.type != USB_RAW_EVENT_CONTROL) {
//		printf("ep0_loop(): event.inner.type != USB_RAW_EVENT_CONTROL, event.inner.type = %d\n", event.inner.type);
//		return false;//continue;
//	}
	
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
										0);
		
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


static void cb_transfer_int_out(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
	
	//	printf("xfr Success!\n");
	epInfo->busyPackets--;
	libusb_free_transfer(xfr);
}
void ep_out_work_interrupt( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 1) {
		usleep(1000);
		return;
	}
	
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;//ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = epInfo->usb_endpoint.wMaxPacketSize;
	
	int rv = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
//	printf("ep_out_work_interrupt(): %d - ", rv);
//	for (int i = 0; i < rv; i++) {
//		printf("%02x ", io.inner.data[i]);
//	}
//	printf("\n");
	
	//unsigned char data[48];
	int transferred = rv;
	
	memcpy(epInfo->data, &io.inner.data[0], transferred);
	
//	int r = libusb_interrupt_transfer(epInfo->deviceHandle,
//									  epInfo->usb_endpoint.bEndpointAddress,
//									  epInfo->data,
//									  rv,
//									  &transferred,
//									  0 );
//	if (r != 0) {
//		//	printf("libusb_interrupt_transfer FAIL\n");
//		return;
//	}
	
	epInfo->busyPackets++;
	struct libusb_transfer *transfer;
	transfer = libusb_alloc_transfer(0);
	if (transfer == NULL) {
		printf("ERROR: libusb_alloc_transfer(0) no memory");
	}
	libusb_fill_interrupt_transfer(	transfer,
								   epInfo->deviceHandle,
								   epInfo->usb_endpoint.bEndpointAddress,
								   epInfo->data,
								   epInfo->usb_endpoint.wMaxPacketSize,
								   cb_transfer_int_out,
								   epInfo,
								   0 );

	if(libusb_submit_transfer(transfer) != LIBUSB_SUCCESS) {
		printf("ERROR! libusb_submit_transfer(transfer) in ep_in_work_interrupt()\n");
		exit(EXIT_FAILURE);
	}
}

static void cb_transfer_int_in(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
	
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;
	io.inner.flags = 0;
	io.inner.length = xfr->actual_length;//0;//epInfo->wMaxPacketSize;
	
	// TODO: everything, really
	//printf("Sending to ep 0x%02x %d\n", io.inner.ep, io.inner.length);
	//			memcpy(&io.inner.data[0], epInfo->data, pack->actual_length);
	memcpy(&io.inner.data[0], xfr->buffer, xfr->actual_length);
	
	int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
	
	epInfo->busyPackets--;
	libusb_free_transfer(xfr);
}
void ep_in_work_interrupt( EndpointInfo* epInfo ) {
//	struct usb_raw_int_io io;
//	io.inner.ep = epInfo->ep_int;//ep_int_in;// | 0X04;
//	io.inner.flags = 0;
//	io.inner.length = epInfo->usb_endpoint.wMaxPacketSize;
//	//printf("$$$$$$$$$ reading from lusbusb 0x%02x length: %d\n", epInfo->usb_endpoint.bEndpointAddress, epInfo->usb_endpoint.wMaxPacketSize);
//	int transferred = epInfo->usb_endpoint.wMaxPacketSize;
//	int r = libusb_interrupt_transfer(epInfo->deviceHandle,
//									  epInfo->usb_endpoint.bEndpointAddress,
//									  epInfo->data,
//									  epInfo->usb_endpoint.wMaxPacketSize,
//									  &transferred,
//									  0 );
//	if (r != 0) {
//		printf("libusb_interrupt_transfer FAIL r = %d\n", r);
//		return;
//	}
//	//printf("Read %d from controller\n", transferred);
//	memcpy(&io.inner.data[0], epInfo->data, transferred);
//	//io.inner.length = transferred;
//
//	int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
//	if (rv < 0) {
//		// error?
//	}
//	if (usbi_handling_events(HANDLE_CTX(dev_handle))){
//		prtinf("usbi_handling_events(HANDLE_CTX(dev_handle))\n");
//	}
	if (epInfo->busyPackets >= 1) {
		//usleep(125);
		return;
	}
	epInfo->busyPackets++;
	struct libusb_transfer *transfer;
	transfer = libusb_alloc_transfer(0);
	if (transfer == NULL) {
		printf("ERROR: libusb_alloc_transfer(0) no memory");
	}
	libusb_fill_interrupt_transfer(	transfer,
								   epInfo->deviceHandle,
								   epInfo->usb_endpoint.bEndpointAddress,
								   epInfo->data,
								   epInfo->usb_endpoint.wMaxPacketSize,
								   cb_transfer_int_in,
								   epInfo,
								   0 );
//	unsigned char *newBuffer = (unsigned char*)malloc(epInfo->usb_endpoint.wMaxPacketSize);
//	if (!newBuffer) {		// also added
//		libusb_free_transfer(transfer);
//		return;// LIBUSB_ERROR_NO_MEM;
//	}
//
//	libusb_fill_bulk_transfer(transfer,
//							  epInfo->deviceHandle,
//							  epInfo->usb_endpoint.bEndpointAddress,
//							  newBuffer,
//							  epInfo->usb_endpoint.wMaxPacketSize,
//							  cb_transfer_int_in,
//							  epInfo,
//							  0);
//		transfer->type = LIBUSB_TRANSFER_TYPE_INTERRUPT;
//
//	if ((epInfo->usb_endpoint.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_OUT) {	// I added this too
//		memcpy( newBuffer, epInfo->data, epInfo->usb_endpoint.wMaxPacketSize);
//	}
//	transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER;	// also added this

	if(libusb_submit_transfer(transfer) != LIBUSB_SUCCESS) {
		printf("ERROR! libusb_submit_transfer(transfer) in ep_in_work_interrupt()\n");
		exit(EXIT_FAILURE);
	}
}

//int yesImDone = true;
static void cb_transfer_iso_in(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		//printf("cb_transfer_iso_in() nmeeds to handle %d iso packets\n",xfr->num_iso_packets );
		for (int i = 0; i < xfr->num_iso_packets; i++) {
			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
				continue;
			}
			
			//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
			
			
			
			
			struct usb_raw_int_io io;
			io.inner.ep = epInfo->ep_int;
			io.inner.flags = 0;
			io.inner.length = pack->actual_length;//0;//epInfo->wMaxPacketSize;
			
			// TODO: everything, really
			//printf("Sending to ep 0x%02x %d\n", io.inner.ep, io.inner.length);
			//			memcpy(&io.inner.data[0], epInfo->data, pack->actual_length);
			memcpy(&io.inner.data[0], xfr->buffer, pack->actual_length);
			
			//			struct pollfd fds[1];
			//			fds[0].fd = epInfo->fd;
			//			fds[0].events = POLLOUT;
			//			int timeout = 1;
			//			int ready;
			//			if ( (ready = poll ( fds, 1, 1 )) == 0 )
			//			{
			//				printf("Not ready to send!\n");
			//				libusb_free_transfer(xfr);
			//				return;
			//			}
			//			printf("Ready: %d\n", ready);
//			int flags;
//			flags = fcntl(epInfo->fd, F_GETFL, 0);
//			if (-1 == flags) {
//				printf("-1 == flagsd %d\n", flags);
//				return ;
//			}
//			fcntl(epInfo->fd, F_SETFL, flags | O_NONBLOCK);
			int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
			if (rv < 0) {
				printf("Error! iso write to host  usb_raw_ep_write() returned %d\n", rv);
			}
			//printf("Dine!\n");
		}
	}
	epInfo->busyPackets--;
	//yesImDone = true;
	libusb_free_transfer(xfr);
}

void ep_in_work_isochronous( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 1) {
		//printf("ep_in_work_isochronous() is waiting on packets!\n");
		usleep(125*8);
		return;
	}
	epInfo->busyPackets++;
	struct libusb_transfer *xfr;
	int num_iso_pack = 1;	// When should this be non-1?
	xfr = libusb_alloc_transfer(num_iso_pack);
	//char buffer[200];
	libusb_fill_iso_transfer(xfr,
							 epInfo->deviceHandle,
							 epInfo->usb_endpoint.bEndpointAddress,
							 //buffer,//
							 epInfo->data,
							 epInfo->usb_endpoint.wMaxPacketSize,
							 num_iso_pack,
							 cb_transfer_iso_in,
							 epInfo,
							 0);
	libusb_set_iso_packet_lengths(xfr, epInfo->usb_endpoint.wMaxPacketSize/num_iso_pack);
	
	libusb_submit_transfer(xfr);
	//yesImDone = false;
	
	//usleep(10000);
}

static void cb_xfr(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
//	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
//		for (int i = 0; i < xfr->num_iso_packets; i++) {
//			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
//
//			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
//				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
//				//exit(5);
//			}
//
//			//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
//		}
//	}
	//	printf("xfr Success!\n");
	epInfo->busyPackets--;
	libusb_free_transfer(xfr);
}
void ep_out_work_isochronous( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 128) {
		//printf("ep_out_work_isochronous() (audio out) is busy with packets\n");
		usleep(125 * 8);
		return;
	}
	epInfo->busyPackets++;
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;
	io.inner.flags = 0;
	io.inner.length = epInfo->usb_endpoint.wMaxPacketSize;
	
	int transferred = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
	if (transferred < 0) {
		printf("No data vailable I guess?\n");
		return;
	}
	
	if (epInfo->busyPackets >= 128) {
		
		epInfo->busyPackets--;
		return;
	}
	//	printf("ep_out_work_isochronous(): %d - ", rv);
	//	for (int i = 0; i < rv; i++) {
	//		printf("%02x ", io.inner.data[i]);
	//	}
	//	printf("\n");
//	int transferred = rv;
//	memcpy(epInfo->data, &io.inner.data[0], transferred);
	
	static struct libusb_transfer *xfr;
	int num_iso_pack = 1;
	xfr = libusb_alloc_transfer(num_iso_pack);
	libusb_fill_iso_transfer(xfr,
							 epInfo->deviceHandle,
							 epInfo->usb_endpoint.bEndpointAddress,
							 &io.inner.data[0],//epInfo->data,
							 transferred,
							 num_iso_pack,
							 cb_xfr,
							 epInfo,
							 0);
	libusb_set_iso_packet_lengths(xfr, transferred/num_iso_pack);
	
	libusb_submit_transfer(xfr);
	
//	if (rv < 0) {
//		// ? error
//	}
}

void* ep_loop_thread( void* data ) {
	EndpointInfo *ep = (EndpointInfo*)data;
	int idleDelay = 1000000;
	int idleCount = 0;
	bool priorenable = false;
	while(ep->keepRunning) {
		if (ep->ep_int >= 0 && !ep->stop) {
			if (priorenable == false &&
				(ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
				priorenable = true;
				usleep(1000000);
				//continue;
			}
			if (ep->usb_endpoint.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) {	// data in
				switch (ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_INTERRUPT:
						ep_in_work_interrupt( ep );
						//usleep(pow(2, ep->usb_endpoint.bInterval-1) * 125);
						//usleep(1000);
						break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
						//usleep(125);
						ep_in_work_isochronous( ep );
						
//						if (idleCount++ > 100) {
//							idleCount = 0;
//							printf("Audio in buffered: ");
//							for (int i = 0; i < ep->busyPackets; i++) {
//								printf(".");
//							}
//							printf("\n");
//						}
						break;
					case LIBUSB_TRANSFER_TYPE_CONTROL:
					case LIBUSB_TRANSFER_TYPE_BULK:
					default:
						printf("Unsupported ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK\n");
						usleep(1000);
						break;
				}
			} else { // data out
				switch (ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_INTERRUPT:
						//usleep(pow(2, ep->usb_endpoint.bInterval-1) * 125);
						//usleep(1000);
						ep_out_work_interrupt( ep );
						break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
						ep_out_work_isochronous( ep );

						if (idleCount++ > 100) {
							idleCount = 0;
							fprintf(stderr, "Audio out buffered: ");
							for (int i = 0; i < ep->busyPackets; i++) {
								fprintf(stderr, ".");
							}
							fprintf(stderr, "    \r");
						}
						//usleep(125);
						break;
					case LIBUSB_TRANSFER_TYPE_CONTROL:
					case LIBUSB_TRANSFER_TYPE_BULK:
					default:
						printf("Unsupported ep->bEndpointAddress\n");
						usleep(1000);
						break;
				}
			}
			
			//usleep(pow(2, ep->usb_endpoint.bInterval-1) * 125);// not sure if binterval should be trusted, nor if it's 125 or 1000.  should also be 2^(interval-1)
		} else {
			idleCount++;
			if (idleCount > 1000000/idleDelay) {
				idleCount = 0;
				printf("Idle: Endpoint 0x%02x\n", ep->usb_endpoint.bEndpointAddress);
			}
			//printf("Idle: Endpoint 0x%02x\n", ep->usb_endpoint.bEndpointAddress);
			//if((ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
			//	usleep(1000000);
			usleep(idleDelay);
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
	
	// this will hold all usb management stuff, build from libusb and provided to raw_gadget when needed.
	EndpointZeroInfo mEndpointZeroInfo;
	pthread_t endpointThreads[60]; // crawl before I walk
	int endpoint = 0;
	
	// libsub setup
	libusb_device **devices;
	libusb_device_handle *deviceHandle;
	libusb_context *context = NULL;
	
	int r = libusb_init(&context);
	if(r < 0) {
		printf("libusb_init() Error %d\n", r);
		return 1;
	}
	//libusb_set_debug(context, 0);
	libusb_set_option(context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
	
	ssize_t deviceCount = libusb_get_device_list(context, &devices);
	if(deviceCount < 0) {
		printf("libusb_get_device_list() Error\n");
		return 1;
	}
	printf("%d Devioces.\n", deviceCount);
	
	int devIndex = -1;
	for (int i = 0; i < deviceCount; i++) {
		printf("Device : %d", i);
		printf(" | Bus %d", libusb_get_bus_number(devices[i]));
		printf(" Port %d\n", libusb_get_port_number(devices[i]));
		//		printf(" - Addr %d\n", libusb_get_device_address(devices[i]));
		
		if (libusb_get_bus_number(devices[i]) == 1 &&	// specific for lower USB2 port on rPi 4 with raspbian
			libusb_get_port_number(devices[i]) == 4) {
			devIndex = i;
			printf(" |-- This is the device of interest!\n");
		}
	}
	if (devIndex < 0) {
		printf("ERROR!  No USB on the lower rPi USB 2.0 port?\n");
		exit(EXIT_FAILURE);
	}
	if( libusb_open(devices[devIndex], &deviceHandle)  != LIBUSB_SUCCESS ) {
		printf("FAILED! libusb_open()\n");
		exit(EXIT_FAILURE);
	}
	printf("Device has been opened!\n");
	//	deviceHandle = libusb_open_device_with_vid_pid(context, VENDOR, PRODUCT); //these are vendorID and productID I found for my usb device
	//	if(deviceHandle == NULL)
	//		cout<<"Cannot open device"<<endl;
	//	else
	//		cout<<"Device Opened"<<endl;
	libusb_free_device_list(devices, 1); //free the list, unref the devices in it
	
	
	
	if(libusb_kernel_driver_active(deviceHandle, 0) == 1) { //find out if kernel driver is attached
		printf("Kernel Driver Active\n");
		if(libusb_detach_kernel_driver(deviceHandle, 0) == 0) //detach it
			printf("Kernel Driver Detached!\n");
	}
	if (libusb_set_auto_detach_kernel_driver(deviceHandle, true) != LIBUSB_SUCCESS) {
		printf("FAILED to perform libusb_set_auto_detach_kernel_driver()\n");
		exit(EXIT_FAILURE);
	}
	
	struct libusb_device_descriptor deviceDescriptor;
	if( libusb_get_device_descriptor(libusb_get_device(deviceHandle), &deviceDescriptor) != LIBUSB_SUCCESS ) {
		printf("FAILED to call libusb_get_device_descriptor()\n");
		exit(EXIT_FAILURE);
	}
	printf("Have Device Descriptor!\n");
	printf(" - bLength            : %d\n",deviceDescriptor.bLength);
	printf(" - bDescriptorType    : %d\n", deviceDescriptor.bDescriptorType);
	printf(      " - bcdUSB             : 0x%04x\n", deviceDescriptor.bcdUSB);
	printf(" - bDeviceClass       : %d\n", deviceDescriptor.bDeviceClass);
	printf(" - bDeviceSubClass    : %d\n", deviceDescriptor.bDeviceSubClass);
	printf(" - bDeviceProtocol    : %d\n", deviceDescriptor.bDeviceProtocol);
	printf(" - bMaxPacketSize0    : %d\n", deviceDescriptor.bMaxPacketSize0);
	printf(      " - idVendor           : 0x%04x\n", deviceDescriptor.idVendor);
	printf(      " - idProduct          : 0x%04x\n", deviceDescriptor.idProduct);
	printf(" - bcdDevice          : %d\n", deviceDescriptor.bcdDevice);
	printf(" - iManufacturer      : %d\n", deviceDescriptor.iManufacturer);
	printf(" - iProduct           : %d\n", deviceDescriptor.iProduct);
	printf(" - iSerialNumber      : %d\n", deviceDescriptor.iSerialNumber);
	printf(" - bNumConfigurations : %d\n", deviceDescriptor.bNumConfigurations);
	
	mEndpointZeroInfo.bNumConfigurations = deviceDescriptor.bNumConfigurations;
	mEndpointZeroInfo.activeConfiguration = -1;
	mEndpointZeroInfo.mConfigurationInfos = (ConfigurationInfo*)malloc( deviceDescriptor.bNumConfigurations * sizeof(ConfigurationInfo));
	mEndpointZeroInfo.fd = fd;
	mEndpointZeroInfo.dev_handle = deviceHandle;
	
	//	if (deviceDescriptor.bNumConfigurations != 1) {
	//		printf("ERROR!  No support for multiple configurations, deviceDescriptor.bNumConfigurations = %d\n", deviceDescriptor.bNumConfigurations);
	//	}
	
	for( int configIndex = 0; configIndex < deviceDescriptor.bNumConfigurations; configIndex++ ) {
		struct libusb_config_descriptor* configDescriptor;
		
		ConfigurationInfo* configInfo = &mEndpointZeroInfo.mConfigurationInfos[configIndex];
		
		if(libusb_get_config_descriptor(libusb_get_device(deviceHandle), configIndex, &configDescriptor) != LIBUSB_SUCCESS) {
			printf("FAILED! libusb_get_config_descriptor()");
			exit(EXIT_FAILURE);
		}
		printf("Have Config Descriptor!");
		printf(" - bLength            : %d\n", configDescriptor->bLength);
		printf(" - bDescriptorType    : %d\n", configDescriptor->bDescriptorType);
		printf(" - wTotalLength       : %d\n", configDescriptor->wTotalLength);
		printf(" - bNumInterfaces     : %d\n", configDescriptor->bNumInterfaces);
		printf(" - bConfigurationValue: %d\n", configDescriptor->bConfigurationValue);
		printf(" - iConfiguration     : %d\n", configDescriptor->iConfiguration);
		printf(" - bmAttributes       : %d\n", configDescriptor->bmAttributes);
		printf(" - MaxPower           : %d\n", configDescriptor->MaxPower);
		printf(" - extra_length       : %d\n", configDescriptor->extra_length);
		
		configInfo->activeInterface = -1;
		configInfo->bNumInterfaces = configDescriptor->bNumInterfaces;
		configInfo->mInterfaceInfos = (InterfaceInfo *) malloc(configDescriptor->bNumInterfaces * sizeof(InterfaceInfo));
		
//		int numInterfaces = configDescriptor->bNumInterfaces;
		int totalEndpoints = 0;
		for (int i = 0; i < configDescriptor->bNumInterfaces; i++) {
			int numAlternates = configDescriptor->interface[i].num_altsetting;
			InterfaceInfo* interfaceInfo = &configInfo->mInterfaceInfos[i];
//			interfaceInfo->active = false;
			interfaceInfo->activeAlternate = -1;
			interfaceInfo->bNumAlternates = numAlternates;
			interfaceInfo->mAlternateInfos = (AlternateInfo*) malloc(numAlternates * sizeof(AlternateInfo));
			
			for (int a = 0; a < numAlternates; a++) {
				const struct libusb_interface_descriptor *interfaceDescriptor = &configDescriptor->interface[i].altsetting[a];
				AlternateInfo* alternateInfo = &interfaceInfo->mAlternateInfos[a];
//				alternateInfo->active = false;
				alternateInfo->bInterfaceNumber = interfaceDescriptor->bInterfaceNumber;
				alternateInfo->bNumEndpoints = interfaceDescriptor->bNumEndpoints;
				alternateInfo->mEndpointInfos = (EndpointInfo *) malloc(interfaceDescriptor->bNumEndpoints * sizeof(EndpointInfo));
				
				printf(" | - Interface %d Alternate %d\n", interfaceDescriptor->bInterfaceNumber, a);
				
				r = libusb_claim_interface(deviceHandle, interfaceDescriptor->bInterfaceNumber);
				if(r < 0) {
					printf("Cannot Claim Interface\n");
					return 1;
				}
				//		printf("Claimed Interface %d\n", configDescriptor->interfaces[i].altsetting->bInterfaceNumber);
				
				totalEndpoints += interfaceDescriptor->bNumEndpoints;
				printf("   - bNumEndpoints      : %d\n", interfaceDescriptor->bNumEndpoints);
				printf("   - Endpoints          : \n");
				for (int e = 0; e < interfaceDescriptor->bNumEndpoints; e++) {
					//libusb_set_interface_alt_setting(deviceHandle, i, a );	// no idea how to use this properly, but putting htis here wrok son a PS5 controller
					const struct libusb_endpoint_descriptor *endpointDescriptor = &interfaceDescriptor->endpoint[e];
					
					EndpointInfo* endpointInfo = &alternateInfo->mEndpointInfos[e];
					endpointInfo->fd = fd;
					endpointInfo->ep_int = -1;
					endpointInfo->deviceHandle = deviceHandle;
					endpointInfo->keepRunning = true;
					endpointInfo->stop = true;
					endpointInfo->busyPackets = 0;
					endpointInfo->usb_endpoint.bLength =	endpointDescriptor->bLength;
					endpointInfo->usb_endpoint.bDescriptorType =	endpointDescriptor->bDescriptorType;
					endpointInfo->usb_endpoint.bEndpointAddress = endpointDescriptor->bEndpointAddress;
					endpointInfo->usb_endpoint.bmAttributes = endpointDescriptor->bmAttributes;
					endpointInfo->usb_endpoint.wMaxPacketSize = endpointDescriptor->wMaxPacketSize;
					endpointInfo->usb_endpoint.bInterval = endpointDescriptor->bInterval;
					endpointInfo->data = (unsigned char*)malloc( endpointDescriptor->wMaxPacketSize * sizeof(unsigned char));
					
					pthread_create(&endpointThreads[endpoint++], NULL, ep_loop_thread, endpointInfo);
					
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
	}
	
	
	//int fd2 = usb_raw_open();
	//usb_raw_init(fd2, USB_SPEED_HIGH, driver, device);
	//usb_raw_run(fd2);
	
	// raw-gadget fun
	printf("Starting raw-gadget");
	usb_raw_init(fd, USB_SPEED_HIGH, driver, device);
	usb_raw_run(fd);
	
	
	// Start ep0 thread afer endpoints, I believe
	printf("Starting ep0 thread");
	pthread_t threadEp0;
	pthread_create(&threadEp0, NULL, ep0_loop_thread, &mEndpointZeroInfo);//fd);
	
	
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	while(1) {
		if(libusb_handle_events_timeout(context, &timeout) != LIBUSB_SUCCESS) {	// needed for iso transfers I believe
			printf("libusb_handle_events() FAILED\n");
		}
		usleep(1);
	}
	close(fd);
	
//	for (int i = 0; i < totalEndpoints; i++) {
//		free(mEndpointInfos[i].data);
//	}
//	free(mEndpointInfos);
	return 0;
}

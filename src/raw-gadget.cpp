
#include "raw-gadget.hpp"

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



void setEndpoint(AlternateInfo* info, int endpoint, bool enable) {
	EndpointInfo* endpointInfo = &info->mEndpointInfos[endpoint];
	if (enable) {
		printf(" - - - Attempting to enable EP 0x%02x\n", endpointInfo->usb_endpoint.bEndpointAddress);
		endpointInfo->ep_int = usb_raw_ep_enable(endpointInfo->fd, &endpointInfo->usb_endpoint);
		//libusb_set_interface_alt_setting
		//usb_raw_ep_set_halt(endpointInfo->fd, endpointInfo->ep_int);
		endpointInfo->stop = false;
		endpointInfo->keepRunning = true;
		pthread_create(&endpointInfo->thread, NULL, ep_loop_thread, endpointInfo);
	} else {	// may need mutex here
		int temp = endpointInfo->ep_int;
		endpointInfo->stop = true;
		endpointInfo->keepRunning = false;
//		while(endpointInfo->busyPackets > 0) {
//			usleep(100);
//		}
		pthread_join(endpointInfo->thread, NULL);
		
		printf(" - - - Attempting to disable EP with: %d\n", temp);
		int ret = usb_raw_ep_disable(endpointInfo->fd, temp);
		printf(" - - - No idea about this ep disable return: %d\n", ret);
		endpointInfo->ep_int = ret;
		
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


//char dummyBuffer[4096];
bool ep0_request(EndpointZeroInfo* info, struct usb_raw_control_event *event,
				 struct usb_raw_control_io *io, bool *done) {
	int r;
	
	io->inner.length = event->ctrl.wLength;
//	if (event->ctrl.bRequestType & 0x80) {
//		printf("copying %d bytes\n", event->ctrl.wLength);
//		//		memcpy(dummyBuffer, &event->inner.data[0], event->ctrl.wLength);
//
//		r = libusb_control_transfer(	info->dev_handle,
//									event->ctrl.bRequestType,
//									event->ctrl.bRequest,
//									event->ctrl.wValue,
//									event->ctrl.wIndex,
//									//								event->inner.data,
//									//									dummyBuffer,
//									(unsigned char*)&io->data[0],
//									event->ctrl.wLength,
//									0);
//		if (r < 0) {
//			printf("libusb_control_transfer error: %s\n", libusb_error_name(r));
//			return false;
//		}
//
//		//	memcpy(&io->inner.data[0], dummyBuffer, r);
//		//		memcpy(&io->data[0], dummyBuffer, r);
//		//	memcpy(&io->inner.data[0], event->inner.data, r);
//		io->inner.length = r;
//	}
	
	if( (event->ctrl.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch(event->ctrl.bRequest) {
			case USB_REQ_SET_CONFIGURATION:
				printf(" - Setting Configuration to: %d\n",  event->ctrl.wValue & 0xff);	// "The lower byte of the wValue field specifies the desired configuration"
				
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
		printf("copying %d bytes\n", event.ctrl.wLength);
		//		memcpy(dummyBuffer, &event->inner.data[0], event->ctrl.wLength);
		
		rv = libusb_control_transfer(	info->dev_handle,
									event.ctrl.bRequestType,
									event.ctrl.bRequest,
									event.ctrl.wValue,
									event.ctrl.wIndex,
									//								event->inner.data,
									//									dummyBuffer,
									(unsigned char*)&io.data[0],
									event.ctrl.wLength,
									0);
		if (rv < 0) {
			printf("libusb_control_transfer error: %s\n", libusb_error_name(rv));
			printf("ep0: stalling\n");
		 usb_raw_ep0_stall(info->fd);
			return false;
		}
		
		//	memcpy(&io->inner.data[0], dummyBuffer, r);
		//		memcpy(&io->data[0], dummyBuffer, r);
		//	memcpy(&io->inner.data[0], event->inner.data, r);
		io.inner.length = rv;
		rv = usb_raw_ep0_write(info->fd, (struct usb_raw_ep_io *)&io);
		printf("ep0: transferred %d bytes (in: DEVICE -> HOST) - ", rv);
	} else {
		rv = usb_raw_ep0_read(info->fd, (struct usb_raw_ep_io *)&io);
		printf("ep0: transferred %d bytes (out: HOST -> DEVICE) - ", rv);
		
		
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


static void cb_transfer_out(struct libusb_transfer *xfr) {
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
	epInfo->busyPackets--;
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		
		
		return;
	}
	
	//	printf("xfr Success!\n");
	//epInfo->busyPackets--;
	libusb_free_transfer(xfr);
}
void ep_out_work_interrupt( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 1) {
		usleep(epInfo->bIntervalInMicroseconds);
		return;
	}
	
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;//ep_int_in;// | 0X04;
	io.inner.flags = 0;
	io.inner.length = epInfo->usb_endpoint.wMaxPacketSize;
	
	int transferred = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
	//	printf("ep_out_work_interrupt(): %d - ", rv);
	//	for (int i = 0; i < rv; i++) {
	//		printf("%02x ", io.inner.data[i]);
	//	}
	//	printf("\n");
	if (transferred <= 0) {	// Shoudl we stil relay a packet of size 0?
		if (transferred < 0) {
			//printf("WARNING: usb_raw_ep_read() returned %d in ep_out_work_interrupt()\n", transferred);
			static int errorCount = 0;
			if (errorCount++ > 100) {
				errorCount = 0;
				printf("WARNING: usb_raw_ep_read() has seen another 100 timeouts\n");
			}
		}
		usleep(epInfo->bIntervalInMicroseconds);
		return;
	}
	//unsigned char data[48];
	//int transferred = rv;
	
	//memcpy(epInfo->data, &io.inner.data[0], transferred);
	
	//epInfo->busyPackets++;
	struct libusb_transfer *transfer;
	transfer = libusb_alloc_transfer(0);
	if (transfer == NULL) {
		printf("ERROR: libusb_alloc_transfer(0) no memory");
	}
	switch(epInfo->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
		case LIBUSB_TRANSFER_TYPE_INTERRUPT:
			libusb_fill_interrupt_transfer(	transfer,
										   epInfo->deviceHandle,
										   epInfo->usb_endpoint.bEndpointAddress,
										   &io.inner.data[0],//epInfo->data,
										   transferred,//epInfo->usb_endpoint.wMaxPacketSize,
										   cb_transfer_out,
										   epInfo,
										   0 );
			break;
		case LIBUSB_TRANSFER_TYPE_BULK:
			libusb_fill_bulk_transfer(	transfer,
									  epInfo->deviceHandle,
									  epInfo->usb_endpoint.bEndpointAddress,
									  &io.inner.data[0],//epInfo->data,
									  transferred,//epInfo->usb_endpoint.wMaxPacketSize,
									  cb_transfer_out,
									  epInfo,
									  0 );
			break;
		default:
			printf("ERROR! p_out_work_interrupt() unknopwn transfer type\n");
			return;
	}
	
	
	epInfo->busyPackets++;
	if(libusb_submit_transfer(transfer) != LIBUSB_SUCCESS) {
		printf("ERROR! libusb_submit_transfer(transfer) in ep_out_work_interrupt()\n");
		exit(EXIT_FAILURE);
	}
}

static void cb_transfer_in(struct libusb_transfer *xfr) {
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "transfer status %d\n", xfr->status);
		return;
	}
	
	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;
	io.inner.flags = 0;
	
	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
		//printf("cb_transfer_iso_in() nmeeds to handle %d iso packets\n",xfr->num_iso_packets );
		for (int i = 0; i < xfr->num_iso_packets; i++) {
			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
			
			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
				continue;
			}
			
			//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
			
			
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
//			int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
			int rv = pack->actual_length;//usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
			if (rv < 0) {
				printf("Error! iso write to host  usb_raw_ep_write() returned %d\n", rv);
			} else if (rv != pack->actual_length) {
				printf("WARNSSSING!   Only sent %d bytes instead of %d\n", rv, pack->actual_length);
			}
			//printf("Dine!\n");
		}
	} else {
		io.inner.length = xfr->actual_length;//0;//epInfo->wMaxPacketSize;
		
		//printf("Sending to ep 0x%02x %d\n", io.inner.ep, io.inner.length);
		//			memcpy(&io.inner.data[0], epInfo->data, pack->actual_length);
		memcpy(&io.inner.data[0], xfr->buffer, xfr->actual_length);
		
		int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
		if (rv < 0) {
			printf("Error! bulk/interrupt write to host  usb_raw_ep_write() returned %d\n", rv);
		} else if (rv != xfr->actual_length) {
			printf("WARNING!   Only sent %d bytes instead of %d\n", rv, xfr->actual_length);
		}
	}
	
	epInfo->busyPackets--;
	libusb_free_transfer(xfr);
}
void ep_in_work_interrupt( EndpointInfo* epInfo ) {

	if (epInfo->busyPackets >= 1) {
		usleep(epInfo->bIntervalInMicroseconds);
		return;
	}
	epInfo->busyPackets++;
	struct libusb_transfer *transfer;
	transfer = libusb_alloc_transfer(0);
	if (transfer == NULL) {
		printf("ERROR: libusb_alloc_transfer(0) no memory");
	}
	switch(epInfo->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
		case LIBUSB_TRANSFER_TYPE_INTERRUPT:
			libusb_fill_interrupt_transfer(	transfer,
										   epInfo->deviceHandle,
										   epInfo->usb_endpoint.bEndpointAddress,
										   epInfo->data,
										   epInfo->usb_endpoint.wMaxPacketSize,
										   cb_transfer_in,
										   epInfo,
										   0 );
			break;
		case LIBUSB_TRANSFER_TYPE_BULK:	// TODO: need to accounf fo bulk streams maybe
			libusb_fill_bulk_transfer(	transfer,
										   epInfo->deviceHandle,
										   epInfo->usb_endpoint.bEndpointAddress,
										   epInfo->data,
										   epInfo->usb_endpoint.wMaxPacketSize,
										   cb_transfer_in,
										   epInfo,
										   0 );
			
			break;
		default:
			printf("ERROR! ep_in_work_interrupt) unknopwn transfer type\n");
			return;
	}

	if(libusb_submit_transfer(transfer) != LIBUSB_SUCCESS) {
		printf("ERROR! libusb_submit_transfer(transfer) in ep_in_work_interrupt()\n");
		exit(EXIT_FAILURE);
	}
}

////int yesImDone = true;
//static void cb_transfer_iso_in(struct libusb_transfer *xfr) {
//	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
//		fprintf(stderr, "transfer status %d\n", xfr->status);
//		return;
//	}
//	EndpointInfo* epInfo = (EndpointInfo*)xfr->user_data;
//	if (xfr->type == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
//		//printf("cb_transfer_iso_in() nmeeds to handle %d iso packets\n",xfr->num_iso_packets );
//		for (int i = 0; i < xfr->num_iso_packets; i++) {
//			struct libusb_iso_packet_descriptor *pack = &xfr->iso_packet_desc[i];
//
//			if (pack->status != LIBUSB_TRANSFER_COMPLETED) {
//				fprintf(stderr, "Error: pack %u status %d\n", i, pack->status);
//				continue;
//			}
//
//			//			printf("pack%u length:%u, actual_length:%u\n", i, pack->length, pack->actual_length);
//
//
//
//
//			struct usb_raw_int_io io;
//			io.inner.ep = epInfo->ep_int;
//			io.inner.flags = 0;
//			io.inner.length = pack->actual_length;//0;//epInfo->wMaxPacketSize;
//
//			// TODO: everything, really
//			//printf("Sending to ep 0x%02x %d\n", io.inner.ep, io.inner.length);
//			//			memcpy(&io.inner.data[0], epInfo->data, pack->actual_length);
//			memcpy(&io.inner.data[0], xfr->buffer, pack->actual_length);
//
//			//			struct pollfd fds[1];
//			//			fds[0].fd = epInfo->fd;
//			//			fds[0].events = POLLOUT;
//			//			int timeout = 1;
//			//			int ready;
//			//			if ( (ready = poll ( fds, 1, 1 )) == 0 )
//			//			{
//			//				printf("Not ready to send!\n");
//			//				libusb_free_transfer(xfr);
//			//				return;
//			//			}
//			//			printf("Ready: %d\n", ready);
////			int flags;
////			flags = fcntl(epInfo->fd, F_GETFL, 0);
////			if (-1 == flags) {
////				printf("-1 == flagsd %d\n", flags);
////				return ;
////			}
////			fcntl(epInfo->fd, F_SETFL, flags | O_NONBLOCK);
//			int rv = usb_raw_ep_write(epInfo->fd, (struct usb_raw_ep_io *)&io);
//			if (rv < 0) {
//				printf("Error! iso write to host  usb_raw_ep_write() returned %d\n", rv);
//			}
//			//printf("Dine!\n");
//		}
//	}
//	epInfo->busyPackets--;
//	//yesImDone = true;
//	libusb_free_transfer(xfr);
//}

void ep_in_work_isochronous( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 1) {
		//printf("ep_in_work_isochronous() is waiting on packets!\n");
		usleep(epInfo->bIntervalInMicroseconds);
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
							 cb_transfer_in,
							 epInfo,
							 0);
	libusb_set_iso_packet_lengths(xfr, epInfo->usb_endpoint.wMaxPacketSize/num_iso_pack);
	
	libusb_submit_transfer(xfr);
	//yesImDone = false;
	
	//usleep(10000);
}

void ep_out_work_isochronous( EndpointInfo* epInfo ) {
	if (epInfo->busyPackets >= 128) {
		//printf("ep_out_work_isochronous() (audio out) is busy with packets\n");
		usleep(epInfo->bIntervalInMicroseconds);
		return;
	}
	epInfo->busyPackets++;
	struct usb_raw_int_io io;
	io.inner.ep = epInfo->ep_int;
	io.inner.flags = 0;
	io.inner.length = epInfo->usb_endpoint.wMaxPacketSize;
	static int errorCount = 0;
	int transferred = usb_raw_ep_read(epInfo->fd, (struct usb_raw_ep_io *)&io);
	if (transferred <= 0) {
		if (errorCount++ % 50) {
			printf("ep_out_work_isochronous() error count %d: No data available I guess? transferred = %d\n", errorCount, transferred);
			
		}
		usleep(epInfo->bIntervalInMicroseconds);
		epInfo->busyPackets--;
		return;
	}
	
//	if (epInfo->busyPackets >= 3) {
//
//		epInfo->busyPackets--;
//		return;
//	}
//		printf("ep_out_work_isochronous(): %d - ", transferred);
//		for (int i = 0; i < transferred; i++) {
//			printf("%02x ", io.inner.data[i]);
//		}
//		printf("\n");
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
							 cb_transfer_out,
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
	
	printf("Starting thread for endpoint 0x%02x\n", ep->usb_endpoint.bEndpointAddress);
	int idleDelay = 1000000;
	int idleCount = 0;
	bool priorenable = false;
	while(ep->keepRunning || (ep->busyPackets > 0)) {
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
					case LIBUSB_TRANSFER_TYPE_BULK:
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
					default:
						printf("Unsupported ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK\n");
						usleep(1000);
						break;
				}
			} else { // data out
				switch (ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) {
					case LIBUSB_TRANSFER_TYPE_INTERRUPT:
					case LIBUSB_TRANSFER_TYPE_BULK:
						//usleep(pow(2, ep->usb_endpoint.bInterval-1) * 125);
						//usleep(1000);
						ep_out_work_interrupt( ep );
						break;
					case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
						ep_out_work_isochronous( ep );

						if (idleCount++ > 300) {
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
					default:
						printf("Unsupported ep->bEndpointAddress\n");
						usleep(1000);
						break;
				}
			}
			
			//usleep(pow(2, ep->usb_endpoint.bInterval-1) * 125);// not sure if binterval should be trusted, nor if it's 125 or 1000.  should also be 2^(interval-1)
		} else {	// reaching here means we are simply cleaning things up
			idleCount++;
			if (idleCount > 1000000/idleDelay) {
				idleCount = 0;
				printf("Idle: Endpoint 0x%02x - ep->busyPackets=%d\n", ep->usb_endpoint.bEndpointAddress, ep->busyPackets);
			}
			//printf("Idle: Endpoint 0x%02x\n", ep->usb_endpoint.bEndpointAddress);
			//if((ep->usb_endpoint.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
			//	usleep(1000000);
			usleep(idleDelay);
		}
	}
	
	printf("Terminating thread for endpoint 0x%02x\n", ep->usb_endpoint.bEndpointAddress);
	
	return NULL;
}


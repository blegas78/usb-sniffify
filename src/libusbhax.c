// SPDX-License-Identifier: GPL-2.0-only

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <libusb.h>

typedef struct _ControlTransfer {
	uint8_t 	bmRequestType;
	uint8_t 	bRequest;
	uint16_t 	wValue;
	uint16_t 	wIndex;
//	unsigned char * 	data,
	uint16_t 	wLength;
//	unsigned int 	timeout
} ControlTransfer;

// From a PS5 -> ds4 controller
//#define VENDOR 0x054c
//#define PRODUCT 0x09cc
//ControlTransfer mControlTransfers[] = {
//	{ 0x80, 0x6, 0x100, 0x0, 18},
//	{ 0x80, 0x6, 0x300, 0x0, 2},
//	{ 0x80, 0x6, 0x300, 0x0, 4},
//	{ 0x80, 0x6, 0x301, 0x409, 2},
//	{ 0x80, 0x6, 0x301, 0x409, 62},
//	{ 0x80, 0x6, 0x302, 0x409, 2},
//	{ 0x80, 0x6, 0x302, 0x409, 40},
//	{ 0x80, 0x6, 0x200, 0x0, 9},
//	{ 0x80, 0x6, 0x200, 0x0, 41},
//	{ 0x00, 0x9, 0x1, 0x0, 0},
//	{ 0x81, 0x6, 0x2200, 0x0, 507},
//	{ 0x21, 0xa, 0x0, 0x0, 0},
//	{ 0xa1, 0x1, 0x302, 0x0, 37},
//	{ 0xa1, 0x1, 0x3a3, 0x0, 49},
//	{ 0xa1, 0x1, 0x312, 0x0, 16},
//	{ 0x21, 0x9, 0x313, 0x0, 23},
//	{ 0x21, 0x9, 0x314, 0x0, 17}
//};
//char length23[] = {0x13,0x85 ,0xcc ,0x44 ,0xc1 ,0x98 ,0x1c ,0x1d ,0x1e ,0x01 ,0x81 ,0xf3 ,0x56 ,0xd1 ,0xdf ,0x24 ,0x8c ,0x1a ,0xdc ,0x05 ,0xbe ,0x1b ,0x2d
//
//};
//
//char length17[] = { 0x14  ,0x02  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00};

//// From ps5 -> magicpro
//#define VENDOR 0x2f24
//#define PRODUCT 0x00f8
//ControlTransfer mControlTransfers[] = {
//	{ 0x80, 0x6, 0x100, 0x0, 18},
//	{ 0x80, 0x6, 0x300, 0x0, 2},
//	{ 0x80, 0x6, 0x300, 0x0, 4},
//	{ 0x80, 0x6, 0x301, 0x409, 2},
//	{ 0x80, 0x6, 0x301, 0x409, 4},
//	{ 0x80, 0x6, 0x302, 0x409, 2},
//	{ 0x80, 0x6, 0x302, 0x409, 24},
//	{ 0x80, 0x6, 0x200, 0x0, 9},
//	{ 0x80, 0x6, 0x200, 0x0, 41},
//	{ 0x00, 0x9, 0x1, 0x0, 0},
//	{ 0x81, 0x6, 0x2200, 0x0, 160},
//	{ 0x21, 0xa, 0x0, 0x0, 0},
//	{ 0xa1, 0x1, 0x303, 0x0, 48}
//};

// From ps5 -> ps5
#define VENDOR 0x054c
#define PRODUCT 0x0ce6
ControlTransfer mControlTransfers[] = {
	{ 0x80, 0x6, 0x100, 0x0, 18},
	{ 0x80, 0x6, 0x300, 0x0, 2},
	{ 0x80, 0x6, 0x300, 0x0, 4},
	{ 0x80, 0x6, 0x301, 0x409, 2},
	{ 0x80, 0x6, 0x301, 0x409, 62},
	{ 0x80, 0x6, 0x302, 0x409, 2},
	{ 0x80, 0x6, 0x302, 0x409, 40},
	{ 0x80, 0x6, 0x200, 0x0, 9},
	{ 0x80, 0x6, 0x200, 0x0, 227},
	{ 0x00, 0x9, 0x1, 0x0, 0},
	{ 0xa1, 0x82, 0x200, 0x500, 2},
	{ 0xa1, 0x83, 0x200, 0x500, 2},
	{ 0xa1, 0x84, 0x200, 0x500, 2},
	{ 0xa1, 0x82, 0x200, 0x200, 2},
	{ 0xa1, 0x83, 0x200, 0x200, 2},
	{ 0xa1, 0x84, 0x200, 0x200, 2},
	{ 0x21, 0x1, 0x200, 0x200, 2},
	{ 0x1, 0xb, 0x1, 0x1, 0},
	{ 0x1, 0xb, 0x1, 0x2, 0},
	
	{ 0x81, 0x6, 0x2200, 0x3, 273},
	{ 0x21, 0xa, 0x0, 0x3, 0},
	{ 0xa1, 0x1, 0x305, 0x3, 41},
	{ 0xa1, 0x1, 0x320, 0x3, 64},
	{ 0xa1, 0x1, 0x309, 0x3, 16} ,
	{ 0x21, 0x9, 0x30a, 0x0, 23},
	{ 0x21, 0x9, 0x308, 0x0, 17}
	
};
//char length23[] = {0x0a, 0x85 , 0xcc , 0x44 , 0xc1 , 0x98 , 0x1c , 0xd6 , 0xcb , 0xb9 , 0x9c , 0x5f , 0x78 , 0x66 , 0x41 , 0x6b , 0xb3 , 0xa3 , 0xdc , 0xca , 0x2e , 0xe4 , 0xbc};
char length23[] = {0x0a , 0x85 , 0xcc , 0x44 , 0xc1 , 0x98 , 0x1c , 0xa9 , 0xdf , 0x91 , 0x5b , 0x4f , 0x9d , 0xa7 , 0xfc , 0x39 , 0xdc , 0x36 , 0xfb , 0xbe , 0x03 , 0x80 , 0xa7};

char length17[] = { 0x08  ,0x01  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00  ,0x00};
char length2[] = {0xe6, 0xe6};

int main() {
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
		printf(" Port %d", libusb_get_port_number(devices[i]));
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
	
	if (deviceDescriptor.bNumConfigurations != 1) {
		printf("ERROR!  No support for multiple configurations, deviceDescriptor.bNumConfigurations = %d\n", deviceDescriptor.bNumConfigurations);
	}
	
	int configIndex = 0;
	struct libusb_config_descriptor* configDescriptor;
	
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
	
	
	int numInterfaces = configDescriptor->bNumInterfaces;
	int totalEndpoints = 0;
	for (int i = 0; i < numInterfaces; i++) {
		int numAlternates = configDescriptor->interface[i].num_altsetting;
		
		for (int a = 0; a < numAlternates; a++) {
			const struct libusb_interface_descriptor *interfaceDescriptor = &configDescriptor->interface[i].altsetting[a];
			printf(" | - Interface %d Alternate %d\n", interfaceDescriptor->bInterfaceNumber, a);
			
			r = libusb_claim_interface(deviceHandle, interfaceDescriptor->bInterfaceNumber); //claim interface 0 (the first) of device (mine had jsut 1)
			if(r < 0) {
				printf("Cannot Claim Interface\n");
				return 1;
			}
			//		printf("Claimed Interface %d\n", configDescriptor->interfaces[i].altsetting->bInterfaceNumber);
			
			totalEndpoints += interfaceDescriptor->bNumEndpoints;
			printf("   - bNumEndpoints      : %d\n", interfaceDescriptor->bNumEndpoints);
			printf("   - Endpoints          : \n");
			for (int e = 0; e < interfaceDescriptor->bNumEndpoints; e++) {
				libusb_set_interface_alt_setting(deviceHandle, i, a );	// no idea how to use this properly, but putting htis here wrok son a PS5 controller
				
				const struct libusb_endpoint_descriptor *endpointDescriptor = &interfaceDescriptor->endpoint[e];
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
	
//	r = libusb_set_interface_alt_setting(deviceHandle, 3, 1);
//	if(r != 0) {
//		std::cout << "Cannot configure alternate setting" << std::endl;
//		return 3;
//	}

	
	
	//r = libusb_bulk_transfer(deviceHandle, (2 | LIBUSB_ENDPOINT_OUT), data, 4, &actual, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
	
	unsigned char data[256];
	
	int count = sizeof(mControlTransfers)/sizeof(mControlTransfers[0]);
	printf("count = %d\n", count);
	
	for(int i = 0; i < count * 0; i++) {
		memset(data, 0, mControlTransfers[i].wLength);
		if (mControlTransfers[i].wLength == 23) {	// for PS5->DS4
			memcpy(data, length23, 23);
		}
		if (mControlTransfers[i].wLength == 2) {	// for PS5->DS4
			memcpy(data, length2, 2);
		}
		else if (mControlTransfers[i].wLength == 17) {
			memcpy(data, length17, 17);
		}
		r = libusb_control_transfer(	deviceHandle,
									mControlTransfers[i].bmRequestType,
									mControlTransfers[i].bRequest,
									mControlTransfers[i].wValue,
									mControlTransfers[i].wIndex,
									data,
									mControlTransfers[i].wLength,
									0);
		
		printf("Requested %d, got %d : ", mControlTransfers[i].wLength, r);
		for (int j = 0; j < r; j++) {
			printf("%02x ", data[j]);
		}
		printf("\n");
	}
	
	// the following may be responislbe for a PS5 asking a DS4 to pair over BT
//	while(1) {
//		sleep(1);
//		memset(data, 0, mControlTransfers[16].wLength);
//		memcpy(data, length17, 17);
//		data[1] = 1;
//		r = libusb_control_transfer(	deviceHandle,
//									mControlTransfers[16].bmRequestType,
//									mControlTransfers[16].bRequest,
//									mControlTransfers[16].wValue,
//									mControlTransfers[16].wIndex,
//									data,
//									mControlTransfers[16].wLength,
//									0);
//
//		printf("Requested %d, got %d : ", mControlTransfers[16].wLength, r);
//		for (int i = 0; i < r; i++) {
//			printf("%02x ", data[i]);
//		}
//		printf("\n");
//	}
//	if(r == 0 && actual == 4) //we wrote the 4 bytes successfully
//		cout<<"Writing Successful!"<<endl;
//	else
//		cout<<"Write Error"<<endl;
//	int wLength = 17;
//	memset(data, 0, wLength);
//	r = libusb_control_transfer	(	deviceHandle,
//									 0x21,
//		  0x09,
//								 0x314,
//		  0,
//		  data,
//		  wLength,
//		  0
//								 );
//	printf("Requested %d, got %d : ", wLength, r);
//	for (int i = 0; i < r; i++) {
//		printf("%02x ", data[i]);
//	}
//	printf("\n");
	
	r = libusb_release_interface(deviceHandle, 0);	// need to do wthis for all calimed interfaces
	if(r!=0) {
		printf("Cannot Release Interface\n");
		return 1;
	}
	printf("Released Interface\n");

	libusb_close(deviceHandle);
	libusb_exit(context);

	return 0;
}

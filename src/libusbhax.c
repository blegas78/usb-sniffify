// SPDX-License-Identifier: GPL-2.0-only

#include <iostream>
#include <libusb.h>
#include <cstring>
#include <unistd.h>

using namespace std;

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
		std::cout << "libusb_init() Error " << r << std::endl;
		return 1;
	}
	//libusb_set_debug(context, 0);

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
			
			
			std::cout << "   - bNumEndpoints      : " << (int)interfaceDescriptor->bNumEndpoints << std::endl;
			std::cout << "   - Endpoints          : " <<std::endl;
			for (int e = 0; e < interfaceDescriptor->bNumEndpoints; e++) {
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
		std::cout << "Cannot Release Interface" << std::endl;
		return 1;
	}
	std::cout << "Released Interface" << std::endl;

	libusb_close(deviceHandle);
	libusb_exit(context);

	return 0;
}

// SPDX-License-Identifier: GPL-2.0-only

#include "magicpro_generated.h"

#define MAGIC_PRO
/*----------------------------------------------------------------------*/
// DEvice:
#define BCD_USB		0x0200

//#define USB_VENDOR	0x1d6b
//#define USB_PRODUCT	0x0104
//#define USB_VENDOR	0x0525
//#define USB_PRODUCT	0xa4a0
#define USB_VENDOR	0x2f24
#define USB_PRODUCT	0x00f8

#define STRING_ID_MANUFACTURER	1
#define STRING_ID_PRODUCT	2
#define STRING_ID_SERIAL	0
#define STRING_ID_CONFIG	0
#define STRING_ID_INTERFACE	0

#define EP_MAX_PACKET_CONTROL	64
#define EP_MAX_PACKET_INT	64

// struct usb_device_descriptor usb_device = {
#define B_DEVICE_CLASS 0
#define B_DEVICE_SUBCLASS 0
#define B_DEVICE_PROTOCOL 0
#define BCD_DEVICE 0x0100
#define NUM_COFIGURATIONS 1

// Configuraiton Descriptor:
// struct usb_config_descriptor usb_config = {
#define B_NUM_INTERFACES 1
#define B_CONFIGURATION_VALUE 1
#define B_MAX_POWER 0x32*5

#define B_INTERVAL 5

// Assigned dynamically.
#define EP_NUM_INT_IN	0x0

// String list (based on reports)
#define REPORT_ID_BT_MAC 0xff
#define REPORT_ID_CALIBRATION 0xfe
#define REPORT_ID_HARDWARE_FIRMWARE 0xfd
#define REPORT_NOIDEAWTF 0xfc

const char manufacturerString[] = ".";//Sony Interactive Entertainment";
const char productString[] = "MAGIC-S PRO";//Wireless Controller";

const char usb_hid_report[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x05,        // Usage (Game Pad)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0x01,        //   Report ID (1)
	0x09, 0x30,        //   Usage (X)
	0x09, 0x31,        //   Usage (Y)
	0x09, 0x32,        //   Usage (Z)
	0x09, 0x35,        //   Usage (Rz)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x04,        //   Report Count (4)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x39,        //   Usage (Hat switch)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x07,        //   Logical Maximum (7)
	0x35, 0x00,        //   Physical Minimum (0)
	0x46, 0x3B, 0x01,  //   Physical Maximum (315)
	0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x65, 0x00,        //   Unit (None)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x0E,        //   Usage Maximum (0x0E)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x0E,        //   Report Count (14)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
	0x09, 0x20,        //   Usage (0x20)
	0x75, 0x06,        //   Report Size (6)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x33,        //   Usage (Rx)
	0x09, 0x34,        //   Usage (Ry)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x02,        //   Report Count (2)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
	0x09, 0x21,        //   Usage (0x21)
	0x95, 0x36,        //   Report Count (54)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x05,        //   Report ID (5)
	0x09, 0x22,        //   Usage (0x22)
	0x95, 0x1F,        //   Report Count (31)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0x03,        //   Report ID (3)
	0x0A, 0x21, 0x27,  //   Usage (0x2721)
	0x95, 0x2F,        //   Report Count (47)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              // End Collection
	0x06, 0xF0, 0xFF,  // Usage Page (Vendor Defined 0xFFF0)
	0x09, 0x40,        // Usage (0x40)
	0xA1, 0x01,        // Collection (Application)
	0x85, 0xF0,        //   Report ID (-16)
	0x09, 0x47,        //   Usage (0x47)
	0x95, 0x3F,        //   Report Count (63)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0xF1,        //   Report ID (-15)
	0x09, 0x48,        //   Usage (0x48)
	0x95, 0x3F,        //   Report Count (63)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0xF2,        //   Report ID (-14)
	0x09, 0x49,        //   Usage (0x49)
	0x95, 0x0F,        //   Report Count (15)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0xF3,        //   Report ID (-13)
	0x0A, 0x01, 0x47,  //   Usage (0x4701)
	0x95, 0x07,        //   Report Count (7)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              // End Collection

	// 160 bytes
};



featureReport03_t magicMagic = {
	.reportId = 0x03,
	.VEN_GamePad2721 = {
		 0x21 , 0x27 , 0x04 , 0xcd , 0x00 , 0x2c , 0x56 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x0d , 0x0d , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00
	}
};

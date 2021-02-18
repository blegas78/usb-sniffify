// SPDX-License-Identifier: GPL-2.0-only

/*----------------------------------------------------------------------*/

#include "ps4_generated.h"

#define PS4_STRING

#define BCD_USB		0x0200

//#define USB_VENDOR	0x1d6b
//#define USB_PRODUCT	0x0104
//#define USB_VENDOR	0x0525
//#define USB_PRODUCT	0xa4a0
#define USB_VENDOR	0x054c
#define USB_PRODUCT	0x09cc

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

// struct usb_config_descriptor usb_config = {
#define B_NUM_INTERFACES 1
#define B_CONFIGURATION_VALUE 1
#define B_MAX_POWER 0x32*5

#define B_INTERVAL 5

// Assigned dynamically.
#define EP_NUM_INT_IN	0x0

#define REPORT_ID_BT_MAC 0x81
#define REPORT_ID_CALIBRATION 0x02
#define REPORT_ID_HARDWARE_FIRMWARE 0xa3
#define REPORT_NOIDEAWTF 0x12

const char manufacturerString[] = "Sony Interactive Entertainment";
const char productString[] = "Wireless Controller";

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
0x15, 0x00,        //   Logical Minimum (0)
0x25, 0x7F,        //   Logical Maximum (127)
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
0x85, 0x04,        //   Report ID (4)
0x09, 0x23,        //   Usage (0x23)
0x95, 0x24,        //   Report Count (36)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x02,        //   Report ID (2)
0x09, 0x24,        //   Usage (0x24)
0x95, 0x24,        //   Report Count (36)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x08,        //   Report ID (8)
0x09, 0x25,        //   Usage (0x25)
0x95, 0x03,        //   Report Count (3)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x10,        //   Report ID (16)
0x09, 0x26,        //   Usage (0x26)
0x95, 0x04,        //   Report Count (4)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x11,        //   Report ID (17)
0x09, 0x27,        //   Usage (0x27)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x12,        //   Report ID (18)
0x06, 0x02, 0xFF,  //   Usage Page (Vendor Defined 0xFF02)
0x09, 0x21,        //   Usage (0x21)
0x95, 0x0F,        //   Report Count (15)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x13,        //   Report ID (19)
0x09, 0x22,        //   Usage (0x22)
0x95, 0x16,        //   Report Count (22)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x14,        //   Report ID (20)
0x06, 0x05, 0xFF,  //   Usage Page (Vendor Defined 0xFF05)
0x09, 0x20,        //   Usage (0x20)
0x95, 0x10,        //   Report Count (16)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x15,        //   Report ID (21)
0x09, 0x21,        //   Usage (0x21)
0x95, 0x2C,        //   Report Count (44)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x06, 0x80, 0xFF,  //   Usage Page (Vendor Defined 0xFF80)
0x85, 0x80,        //   Report ID (-128)
0x09, 0x20,        //   Usage (0x20)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x81,        //   Report ID (-127)
0x09, 0x21,        //   Usage (0x21)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x82,        //   Report ID (-126)
0x09, 0x22,        //   Usage (0x22)
0x95, 0x05,        //   Report Count (5)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x83,        //   Report ID (-125)
0x09, 0x23,        //   Usage (0x23)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x84,        //   Report ID (-124)
0x09, 0x24,        //   Usage (0x24)
0x95, 0x04,        //   Report Count (4)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x85,        //   Report ID (-123)
0x09, 0x25,        //   Usage (0x25)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x86,        //   Report ID (-122)
0x09, 0x26,        //   Usage (0x26)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x87,        //   Report ID (-121)
0x09, 0x27,        //   Usage (0x27)
0x95, 0x23,        //   Report Count (35)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x88,        //   Report ID (-120)
0x09, 0x28,        //   Usage (0x28)
0x95, 0x22,        //   Report Count (34)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x89,        //   Report ID (-119)
0x09, 0x29,        //   Usage (0x29)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x90,        //   Report ID (-112)
0x09, 0x30,        //   Usage (0x30)
0x95, 0x05,        //   Report Count (5)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x91,        //   Report ID (-111)
0x09, 0x31,        //   Usage (0x31)
0x95, 0x03,        //   Report Count (3)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x92,        //   Report ID (-110)
0x09, 0x32,        //   Usage (0x32)
0x95, 0x03,        //   Report Count (3)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0x93,        //   Report ID (-109)
0x09, 0x33,        //   Usage (0x33)
0x95, 0x0C,        //   Report Count (12)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA0,        //   Report ID (-96)
0x09, 0x40,        //   Usage (0x40)
0x95, 0x06,        //   Report Count (6)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA1,        //   Report ID (-95)
0x09, 0x41,        //   Usage (0x41)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA2,        //   Report ID (-94)
0x09, 0x42,        //   Usage (0x42)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA3,        //   Report ID (-93)
0x09, 0x43,        //   Usage (0x43)
0x95, 0x30,        //   Report Count (48)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA4,        //   Report ID (-92)
0x09, 0x44,        //   Usage (0x44)
0x95, 0x0D,        //   Report Count (13)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA5,        //   Report ID (-91)
0x09, 0x45,        //   Usage (0x45)
0x95, 0x15,        //   Report Count (21)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA6,        //   Report ID (-90)
0x09, 0x46,        //   Usage (0x46)
0x95, 0x15,        //   Report Count (21)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
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
0x85, 0xA7,        //   Report ID (-89)
0x09, 0x4A,        //   Usage (0x4A)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA8,        //   Report ID (-88)
0x09, 0x4B,        //   Usage (0x4B)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xA9,        //   Report ID (-87)
0x09, 0x4C,        //   Usage (0x4C)
0x95, 0x08,        //   Report Count (8)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAA,        //   Report ID (-86)
0x09, 0x4E,        //   Usage (0x4E)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAB,        //   Report ID (-85)
0x09, 0x4F,        //   Usage (0x4F)
0x95, 0x39,        //   Report Count (57)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAC,        //   Report ID (-84)
0x09, 0x50,        //   Usage (0x50)
0x95, 0x39,        //   Report Count (57)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAD,        //   Report ID (-83)
0x09, 0x51,        //   Usage (0x51)
0x95, 0x0B,        //   Report Count (11)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAE,        //   Report ID (-82)
0x09, 0x52,        //   Usage (0x52)
0x95, 0x01,        //   Report Count (1)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xAF,        //   Report ID (-81)
0x09, 0x53,        //   Usage (0x53)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xB0,        //   Report ID (-80)
0x09, 0x54,        //   Usage (0x54)
0x95, 0x3F,        //   Report Count (63)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xB1,        //   Report ID (-79)
0x09, 0x55,        //   Usage (0x55)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xB2,        //   Report ID (-78)
0x09, 0x56,        //   Usage (0x56)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xE0,        //   Report ID (-32)
0x09, 0x57,        //   Usage (0x57)
0x95, 0x02,        //   Report Count (2)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xB3,        //   Report ID (-77)
0x09, 0x55,        //   Usage (0x55)
0x95, 0x3F,        //   Report Count (63)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0x85, 0xB4,        //   Report ID (-76)
0x09, 0x55,        //   Usage (0x55)
0x95, 0x3F,        //   Report Count (63)
0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
0xC0,              // End Collection

// 507 bytes
};

//typedef struct
//{
//  uint8_t  reportId;                                 // Report ID = 0x81 (129) sony_check_add() sony:hid
//													 // Collection: CA:GamePad
//  uint8_t  VEN_GamePad0021[6];                       // Usage 0xFF800021: , Value = 0 to 255, Physical = Value x 21 / 17
//} featureReport81_t;

featureReport81_t blueMacAddress =
{
  .reportId = 0x81,                                 // Report ID = 0x81 (129) sony_check_add() sony:hid
													 // Collection: CA:GamePad
	.VEN_GamePad0021 = {
		0x11, 0x22, 0x33, 0x44, 0x55, 0x66
	},                       // Usage 0xFF800021: , Value = 0 to 255, Physical = Value x 21 / 17
};

//typedef struct
//{
//  uint8_t  reportId;                                 // Report ID = 0x02 (2) dualshock4_get_calibration_data() https://github.com/torvalds/linux/blob/master/drivers/hid/hid-sony.c
//													 // Collection: CA:GamePad
//  uint8_t  VEN_GamePad0024[36];                      // Usage 0xFF000024: , Value = 0 to 255, Physical = Value x 21 / 17
//} featureReport02_t;

featureReport02_t calibrationData =
{
	.reportId = 0x02,
	.VEN_GamePad0024 = {
//		0x00,0x00, //gyro_pitch_bias
//		0x00,0x00, //gyro_yaw_bias
//		0x00,0x00, //gyro_roll_bias
//		0x01,0x00, // pithc plus
//		0x00,0x00, // pithc minus
//		0x01,0x00, // yaw plus
//		0x00,0x00, // yaw minuw
//		0x01,0x00, // roll plus
//		0x00,0x00, // roll minus
//		0x01,0x00, // gyro speed plu
//		0x00,0x00, // gyro speed minus
//		0x01,0x00, // x plus
//		0x00,0x00, // x minus
//		0x01,0x00, // y plus
//		0x00,0x00, // y minus
//		0x01,0x00, // z plus
//		0x00,0x00, // z minus
//		0x00,0x00, // ?
		0x15, 0x00 ,
		0x04 , 0x00 ,
		0x09 , 0x00 ,
		0x0b , 0x22 ,
		0x22 , 0xde ,
		0xb6 , 0x22 ,
		0x4a , 0xdd ,
		0x05 , 0x24 ,
		0x02 , 0xdc ,
		0x1c , 0x02 ,
		0x1c , 0x02 ,
		0xfc , 0x1f ,
		0x03 , 0xe0 ,
		0x83 , 0x20 ,
		0x7d , 0xdf ,
		0xca , 0x1f ,
		0x36 , 0xe0 ,
		0x06 , 0x00
	}
};

//typedef struct
//{
//  uint8_t  reportId;                                 // Report ID = 0xA3 (163) dualshock4_get_version_info() sony:hid
//													 // Collection: CA:GamePad
//  uint8_t  VEN_GamePad0043[48];                      // Usage 0xFF800043: , Value = 0 to 255, Physical = Value x 21 / 17
//} featureReportA3_t;

featureReportA3_t hardFirmVersion = {
	.reportId = 0xa3,
	.VEN_GamePad0043 = {
//		0, 0, 0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0, 0, 0, 0,
//		0, 0, 0, 0, 0, 0, 0, 0,
//		0, 0, 0,
//		0x00, 0x74,
//		0, 0, 0, 0,
//		0x07, 0x80,
//		0, 0, 0, 0, 0
		0x4a, 0x75 , 0x6c , 0x20 , 0x31 , 0x31 , 0x20 , 0x32 , 0x30 , 0x31 , 0x36 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x31 , 0x31 , 0x3a , 0x30 , 0x38 , 0x3a , 0x32 , 0x32 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x74 , 0x01 , 0x00 , 0x00 , 0x00 , 0x07 , 0x80 , 0x00 , 0x05 , 0x00 , 0x80 , 0x03 , 0x00
	}
	
};

//typedef struct
//{
//  uint8_t  reportId;                                 // Report ID = 0x12 (18)
//													 // Collection: CA:GamePad
//  uint8_t  VEN_GamePad0021[15];                      // Usage 0xFF020021: , Value = 0 to 255, Physical = Value x 21 / 17
//} featureReport12_t;

featureReport12_t noIdeaWTF = {
	.reportId = 0x12,
	.VEN_GamePad0021 = {
		0x03 , 0xac , 0x39 , 0x84 ,
		0x20 , 0x70 , 0x08 , 0x25 ,
		0x00 , 0x00 , 0x00 , 0x00 ,
		0x00 , 0x00 , 0x00
	}
};



//typedef struct
//{
//  uint8_t  reportId;                                 // Report ID = 0x01 (1)
//													 // Collection: CA:GamePad
//  uint8_t  GD_GamePadX;                              // Usage 0x00010030: X, Value = 0 to 255
//  uint8_t  GD_GamePadY;                              // Usage 0x00010031: Y, Value = 0 to 255
//  uint8_t  GD_GamePadZ;                              // Usage 0x00010032: Z, Value = 0 to 255
//  uint8_t  GD_GamePadRz;                             // Usage 0x00010035: Rz, Value = 0 to 255
//  uint8_t  GD_GamePadHatSwitch : 4;                  // Usage 0x00010039: Hat switch, Value = 0 to 7, Physical = Value x 45 in degrees
//  uint8_t  BTN_GamePadButton1 : 1;                   // Usage 0x00090001: Button 1 Primary/trigger, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton2 : 1;                   // Usage 0x00090002: Button 2 Secondary, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton3 : 1;                   // Usage 0x00090003: Button 3 Tertiary, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton4 : 1;                   // Usage 0x00090004: Button 4, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton5 : 1;                   // Usage 0x00090005: Button 5, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton6 : 1;                   // Usage 0x00090006: Button 6, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton7 : 1;                   // Usage 0x00090007: Button 7, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton8 : 1;                   // Usage 0x00090008: Button 8, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton9 : 1;                   // Usage 0x00090009: Button 9, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton10 : 1;                  // Usage 0x0009000A: Button 10, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton11 : 1;                  // Usage 0x0009000B: Button 11, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton12 : 1;                  // Usage 0x0009000C: Button 12, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton13 : 1;                  // Usage 0x0009000D: Button 13, Value = 0 to 1, Physical = Value x 315
//  uint8_t  BTN_GamePadButton14 : 1;                  // Usage 0x0009000E: Button 14, Value = 0 to 1, Physical = Value x 315
//  uint8_t  VEN_GamePad0020 : 6;                      // Usage 0xFF000020: , Value = 0 to 127, Physical = Value x 315 / 127
//  uint8_t  GD_GamePadRx;                             // Usage 0x00010033: Rx, Value = 0 to 255, Physical = Value x 21 / 17
//  uint8_t  GD_GamePadRy;                             // Usage 0x00010034: Ry, Value = 0 to 255, Physical = Value x 21 / 17
//  uint8_t  VEN_GamePad0021[54];                      // Usage 0xFF000021: , Value = 0 to 255, Physical = Value x 21 / 17
//} inputReport01_t;

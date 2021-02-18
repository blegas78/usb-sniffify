
//--------------------------------------------------------------------------------
// Decoded Application Collection
//--------------------------------------------------------------------------------

/*
05 01        (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
09 05        (LOCAL)  USAGE              0x00010005 Game Pad (Application Collection)  
A1 01        (MAIN)   COLLECTION         0x01 Application (Usage=0x00010005: Page=Generic Desktop Page, Usage=Game Pad, Type=Application Collection)
85 01          (GLOBAL) REPORT_ID          0x01 (1)  
09 30          (LOCAL)  USAGE              0x00010030 X (Dynamic Value)  
09 31          (LOCAL)  USAGE              0x00010031 Y (Dynamic Value)  
09 32          (LOCAL)  USAGE              0x00010032 Z (Dynamic Value)  
09 35          (LOCAL)  USAGE              0x00010035 Rz (Dynamic Value)  
15 00          (GLOBAL) LOGICAL_MINIMUM    0x00 (0)  <-- Info: Consider replacing 15 00 with 14
26 FF00        (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)  
75 08          (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
95 04          (GLOBAL) REPORT_COUNT       0x04 (4) Number of fields  
81 02          (MAIN)   INPUT              0x00000002 (4 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
09 39          (LOCAL)  USAGE              0x00010039 Hat switch (Dynamic Value)  
15 00          (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
25 07          (GLOBAL) LOGICAL_MAXIMUM    0x07 (7)  
35 00          (GLOBAL) PHYSICAL_MINIMUM   0x00 (0)  <-- Info: Consider replacing 35 00 with 34
46 3B01        (GLOBAL) PHYSICAL_MAXIMUM   0x013B (315)  
65 14          (GLOBAL) UNIT               0x14 Rotation in degrees [1° units] (4=System=English Rotation, 1=Rotation=Degrees)  
75 04          (GLOBAL) REPORT_SIZE        0x04 (4) Number of bits per field  
95 01          (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
81 42          (MAIN)   INPUT              0x00000042 (1 field x 4 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 1=Null 0=NonVolatile 0=Bitmap 
65 00          (GLOBAL) UNIT               0x00 No unit (0=None)  <-- Info: Consider replacing 65 00 with 64
05 09          (GLOBAL) USAGE_PAGE         0x0009 Button Page 
19 01          (LOCAL)  USAGE_MINIMUM      0x00090001 Button 1 Primary/trigger (Selector, On/Off Control, Momentary Control, or One Shot Control)  
29 0E          (LOCAL)  USAGE_MAXIMUM      0x0009000E Button 14 (Selector, On/Off Control, Momentary Control, or One Shot Control)  
15 00          (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
25 01          (GLOBAL) LOGICAL_MAXIMUM    0x01 (1)  
75 01          (GLOBAL) REPORT_SIZE        0x01 (1) Number of bits per field  
95 0E          (GLOBAL) REPORT_COUNT       0x0E (14) Number of fields  
81 02          (MAIN)   INPUT              0x00000002 (14 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
06 00FF        (GLOBAL) USAGE_PAGE         0xFF00 Vendor-defined 
09 20          (LOCAL)  USAGE              0xFF000020 <-- Warning: Undocumented usage (document it by inserting 0020 into file FF00.conf)
75 06          (GLOBAL) REPORT_SIZE        0x06 (6) Number of bits per field  
95 01          (GLOBAL) REPORT_COUNT       0x01 (1) Number of fields  
81 02          (MAIN)   INPUT              0x00000002 (1 field x 6 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
05 01          (GLOBAL) USAGE_PAGE         0x0001 Generic Desktop Page 
09 33          (LOCAL)  USAGE              0x00010033 Rx (Dynamic Value)  
09 34          (LOCAL)  USAGE              0x00010034 Ry (Dynamic Value)  
15 00          (GLOBAL) LOGICAL_MINIMUM    0x00 (0) <-- Redundant: LOGICAL_MINIMUM is already 0 <-- Info: Consider replacing 15 00 with 14
26 FF00        (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)  
75 08          (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field  
95 02          (GLOBAL) REPORT_COUNT       0x02 (2) Number of fields  
81 02          (MAIN)   INPUT              0x00000002 (2 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
06 00FF        (GLOBAL) USAGE_PAGE         0xFF00 Vendor-defined 
09 21          (LOCAL)  USAGE              0xFF000021 <-- Warning: Undocumented usage (document it by inserting 0021 into file FF00.conf)
95 36          (GLOBAL) REPORT_COUNT       0x36 (54) Number of fields  
81 02          (MAIN)   INPUT              0x00000002 (54 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
85 05          (GLOBAL) REPORT_ID          0x05 (5)  
09 22          (LOCAL)  USAGE              0xFF000022 <-- Warning: Undocumented usage (document it by inserting 0022 into file FF00.conf)
95 1F          (GLOBAL) REPORT_COUNT       0x1F (31) Number of fields  
91 02          (MAIN)   OUTPUT             0x00000002 (31 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
85 03          (GLOBAL) REPORT_ID          0x03 (3)  
0A 2127        (LOCAL)  USAGE              0xFF002721 <-- Warning: Undocumented usage (document it by inserting 2721 into file FF00.conf)
95 2F          (GLOBAL) REPORT_COUNT       0x2F (47) Number of fields  
B1 02          (MAIN)   FEATURE            0x00000002 (47 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
C0           (MAIN)   END_COLLECTION     Application  <-- Warning: Physical units are still in effect PHYSICAL(MIN=0,MAX=315) UNIT(0x00000000,EXP=0)
*/

//--------------------------------------------------------------------------------
// Vendor-defined featureReport 03 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x03 (3)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad2721[47];                      // Usage 0xFF002721: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReport03_t;


//--------------------------------------------------------------------------------
// Generic Desktop Page inputReport 01 (Device --> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x01 (1)
                                                     // Collection: CA:GamePad
  uint8_t  GD_GamePadX;                              // Usage 0x00010030: X, Value = 0 to 255
  uint8_t  GD_GamePadY;                              // Usage 0x00010031: Y, Value = 0 to 255
  uint8_t  GD_GamePadZ;                              // Usage 0x00010032: Z, Value = 0 to 255
  uint8_t  GD_GamePadRz;                             // Usage 0x00010035: Rz, Value = 0 to 255
  uint8_t  GD_GamePadHatSwitch : 4;                  // Usage 0x00010039: Hat switch, Value = 0 to 7, Physical = Value x 45 in degrees
  uint8_t  BTN_GamePadButton1 : 1;                   // Usage 0x00090001: Button 1 Primary/trigger, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton2 : 1;                   // Usage 0x00090002: Button 2 Secondary, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton3 : 1;                   // Usage 0x00090003: Button 3 Tertiary, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton4 : 1;                   // Usage 0x00090004: Button 4, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton5 : 1;                   // Usage 0x00090005: Button 5, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton6 : 1;                   // Usage 0x00090006: Button 6, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton7 : 1;                   // Usage 0x00090007: Button 7, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton8 : 1;                   // Usage 0x00090008: Button 8, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton9 : 1;                   // Usage 0x00090009: Button 9, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton10 : 1;                  // Usage 0x0009000A: Button 10, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton11 : 1;                  // Usage 0x0009000B: Button 11, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton12 : 1;                  // Usage 0x0009000C: Button 12, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton13 : 1;                  // Usage 0x0009000D: Button 13, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton14 : 1;                  // Usage 0x0009000E: Button 14, Value = 0 to 1, Physical = Value x 315
  uint8_t  VEN_GamePad0020 : 6;                      // Usage 0xFF000020: , Value = 0 to 1, Physical = Value x 315
  uint8_t  GD_GamePadRx;                             // Usage 0x00010033: Rx, Value = 0 to 255, Physical = Value x 21 / 17
  uint8_t  GD_GamePadRy;                             // Usage 0x00010034: Ry, Value = 0 to 255, Physical = Value x 21 / 17
  uint8_t  VEN_GamePad0021[54];                      // Usage 0xFF000021: , Value = 0 to 255, Physical = Value x 21 / 17
} inputReport01_t;


//--------------------------------------------------------------------------------
// Vendor-defined outputReport 05 (Device <-- Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0x05 (5)
                                                     // Collection: CA:GamePad
  uint8_t  VEN_GamePad0022[31];                      // Usage 0xFF000022: , Value = 0 to 255, Physical = Value x 21 / 17
} outputReport05_t;


//--------------------------------------------------------------------------------
// Decoded Application Collection
//--------------------------------------------------------------------------------

/*
06 F0FF      (GLOBAL) USAGE_PAGE         0xFFF0 Vendor-defined 
09 40        (LOCAL)  USAGE              0xFFF00040 <-- Warning: Undocumented usage (document it by inserting 0040 into file FFF0.conf)
A1 01        (MAIN)   COLLECTION         0x01 Application (Usage=0xFFF00040: Page=Vendor-defined, Usage=, Type=) <-- Error: COLLECTION must be preceded by a known USAGE
85 F0          (GLOBAL) REPORT_ID          0xF0 (240)  
09 47          (LOCAL)  USAGE              0xFFF00047 <-- Warning: Undocumented usage (document it by inserting 0047 into file FFF0.conf)
95 3F          (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields  
B1 02          (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
85 F1          (GLOBAL) REPORT_ID          0xF1 (241)  
09 48          (LOCAL)  USAGE              0xFFF00048 <-- Warning: Undocumented usage (document it by inserting 0048 into file FFF0.conf)
95 3F          (GLOBAL) REPORT_COUNT       0x3F (63) Number of fields <-- Redundant: REPORT_COUNT is already 63 
B1 02          (MAIN)   FEATURE            0x00000002 (63 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
85 F2          (GLOBAL) REPORT_ID          0xF2 (242)  
09 49          (LOCAL)  USAGE              0xFFF00049 <-- Warning: Undocumented usage (document it by inserting 0049 into file FFF0.conf)
95 0F          (GLOBAL) REPORT_COUNT       0x0F (15) Number of fields  
B1 02          (MAIN)   FEATURE            0x00000002 (15 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
85 F3          (GLOBAL) REPORT_ID          0xF3 (243)  
0A 0147        (LOCAL)  USAGE              0xFFF04701 <-- Warning: Undocumented usage (document it by inserting 4701 into file FFF0.conf)
95 07          (GLOBAL) REPORT_COUNT       0x07 (7) Number of fields  
B1 02          (MAIN)   FEATURE            0x00000002 (7 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap 
C0           (MAIN)   END_COLLECTION     Application  <-- Warning: Physical units are still in effect PHYSICAL(MIN=0,MAX=315) UNIT(0x00000000,EXP=0)
*/

//--------------------------------------------------------------------------------
// Vendor-defined featureReport F0 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF0 (240)
                                                     // Collection: CA:
  uint8_t  VEN_0047[63];                             // Usage 0xFFF00047: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF0_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F1 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF1 (241)
                                                     // Collection: CA:
  uint8_t  VEN_0048[63];                             // Usage 0xFFF00048: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF1_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F2 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF2 (242)
                                                     // Collection: CA:
  uint8_t  VEN_0049[15];                             // Usage 0xFFF00049: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF2_t;


//--------------------------------------------------------------------------------
// Vendor-defined featureReport F3 (Device <-> Host)
//--------------------------------------------------------------------------------

typedef struct
{
  uint8_t  reportId;                                 // Report ID = 0xF3 (243)
                                                     // Collection: CA:
  uint8_t  VEN_4701[7];                              // Usage 0xFFF04701: , Value = 0 to 255, Physical = Value x 21 / 17
} featureReportF3_t;


#include "st7735.h"

//-------------------------------------------------------------------------------------------//

const uint8_t Bcmd[] PROGMEM = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay

const uint8_t Rcmd1[] PROGMEM = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color

const uint8_t RcmdBlack[] PROGMEM = {   // Init for 7735R black tab
    14,                     // 14 commands in list:
  ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
    25,                     //     25 ms delay
  ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
    25,                     //     25 ms delay
  ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
    0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  ST7735_PWCTR1 , 3      ,  //  4: Power control, 3 args, no delay:
    0xA2,
    0x02,                   //     -4.6V
    0x84,                   //     AUTO mode
  ST7735_PWCTR2 , 1      ,  //  5: Power control, 1 arg, no delay:
    0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  ST7735_PWCTR3 , 2      ,  //  6: Power control, 2 args, no delay:
    0x0A,                   //     Opamp current small
    0x00,                   //     Boost frequency
  ST7735_PWCTR4 , 2      ,  //  7: Power control, 2 args, no delay:
    0x8A,                   //     BCLK/2, Opamp current small & Medium low
    0x2A,
  ST7735_PWCTR5 , 2      ,  //  8: Power control, 2 args, no delay:
    0x8A, 0xEE,
  ST7735_VMCTR1 , 1      ,  //  9: Power control, 1 arg, no delay:
    0x0E,
  ST7735_MADCTL , 1      ,  // 10: Memory access control (directions), 1 arg:
    (MADCTL_MY | MADCTL_MV | MADCTL_RGB), //     row addr/col addr, bottom to top refresh
  ST7735_COLMOD , 1      ,  // 11: set color mode, 1 arg, no delay:
    0x05,                   //     16-bit color
  ST7735_GMCTRP1, 16      , // 12: Magical unicorn dust, 16 args, no delay:
    0x02, 0x1c, 0x07, 0x12,
    0x37, 0x32, 0x29, 0x2d,
    0x29, 0x25, 0x2B, 0x39,
    0x00, 0x01, 0x03, 0x10,
  ST7735_GMCTRN1, 16      , // 13: Sparkles and rainbows, 16 args, no delay:
    0x03, 0x1d, 0x07, 0x06,
    0x2E, 0x2C, 0x29, 0x2D,
    0x2E, 0x2E, 0x37, 0x3F,
    0x00, 0x00, 0x02, 0x10,
  ST7735_DISPON ,    DELAY, // 14: Main screen turn on, no args w/delay
    25                      //     25 ms delay
};

const uint8_t Rcmd2green[] PROGMEM = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

const uint8_t Rcmd2red[] PROGMEM = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159

const uint8_t Rcmd2green144[] PROGMEM = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F };           //     XEND = 127

const uint8_t Rcmd3[] PROGMEM = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

const uint8_t Gcmd[] PROGMEM = {                  // Initialization commands for 7735B screens
  19,                       // 18 commands in list:
  ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
  50,                     //     50 ms delay
  ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
  100,                    //     255 = 500 ms delay
  0x26 , 1,  			// 3: Set default gamma
  0x04,                     //     16-bit color
  0xb1, 2,              	// 4: Frame Rate
  0x0b,
  0x14,
  0xc0, 2,                    // 5: VRH1[4:0] & VC[2:0]
  0x08,
  0x00,
  0xc1, 1,                    // 6: BT[2:0]
  0x05,
  0xc5, 2,                    // 7: VMH[6:0] & VML[6:0]
  0x41,
  0x30,
  0xc7, 1,                    // 8: LCD Driving control
  0xc1,
  0xEC, 1,                    // 9: Set pumping color freq
  0x1b,
  0x3a , 1 + DELAY,  	        // 10: Set color format
  0x55,                     //     16-bit color
  100,
  0x2a, 4,                    // 11: Set Column Address
  0x00,
  0x00,
  0x00,
  0x7f,
  0x2b, 4,                    // 12: Set Page Address
  0x00,
  0x00,
  0x00,
  0x9f,
  0x36, 1,                    // 12+1: Set Scanning Direction
  0xc8,
  0xb7, 1,			// 14: Set Source Output Direciton
  0x00,
  0xf2, 1,			// 15: Enable Gamma bit
  0x00,
  0xe0, 15 + DELAY,		// 16: magic
  0x28, 0x24, 0x22, 0x31,
  0x2b, 0x0e, 0x53, 0xa5,
  0x42, 0x16, 0x18, 0x12,
  0x1a, 0x14, 0x03,
  50,
  0xe1, 15 + DELAY,		// 17: more magic
  0x17, 0x1b, 0x1d, 0x0e,
  0x14, 0x11, 0x2c, 0xa5,
  0x3d, 0x09, 0x27, 0x2d,
  0x25, 0x2b, 0x3c,
  50,
  ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
  10,                     //     10 ms delay
  ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
  255
};                  //     255 = 500 ms delay

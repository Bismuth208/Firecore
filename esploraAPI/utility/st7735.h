#ifndef _ST7735_H
#define _ST7735_H

#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/pgmspace.h>


// some flags for initR() :(
#define INITR_GREENTAB   0x0
#define INITR_REDTAB     0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160

//-------------------------------------------------------------------------------------------//

#define ST7735_NOP      0x00
#define ST7735_SWRESET  0x01
#define ST7735_RDDID    0x04
#define ST7735_RDDST    0x09

#define ST7735_SLPIN    0x10
#define ST7735_SLPOUT   0x11
#define ST7735_PTLON    0x12
#define ST7735_NORON    0x13

#define ST7735_INVOFF   0x20
#define ST7735_INVON    0x21
#define ST7735_DISPOFF  0x28
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E

#define ST7735_PTLAR    0x30
#define ST7735_VSCRDEF  0x33
#define ST7735_VSCRSADD 0x37
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36

#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_DISSET5  0xB6

#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5

#define ST7735_RDID1    0xDA
#define ST7735_RDID2    0xDB
#define ST7735_RDID3    0xDC
#define ST7735_RDID4    0xDD

#define ST7735_PWCTR6   0xFC

#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1


#define MADCTL_MY       0x80
#define MADCTL_MX       0x40
#define MADCTL_MV       0x20
#define MADCTL_ML       0x10
#define MADCTL_RGB      0x00
#define MADCTL_BGR      0x08
#define MADCTL_MH       0x04


//-------------------------------------------------------------------------------------------//

#define DELAY           0x80  // delay marker

#define TFT_BOT_FIXED_AREA 0  // Number of lines in bottom fixed area (lines counted from bottom of screen)
#define TFT_TOP_FIXED_AREA 0  // Number of lines in top fixed area (lines counted from top of screen)

#define WIDTH  ST7735_TFTWIDTH
#define HEIGHT ST7735_TFTHEIGHT_18

//-------------------------------------------------------------------------------------------//


#define SET_BIT(x, y)       ((x) |= (1 << y));
#define RESET_BIT(x, y)     ((x) &= ~(1 << y));


#define TFT_CS_PIN         PE6
#define TFT_DDRX_CS        DDRE
#define TFT_PORTX_CS       PORTE
#define SET_TFT_CS_HI      SET_BIT(TFT_PORTX_CS, TFT_CS_PIN);
#define SET_TFT_CS_LOW     RESET_BIT(TFT_PORTX_CS, TFT_CS_PIN);

#define TFT_DC_PIN         PD2
#define TFT_DDRX_DC        DDRD
#define TFT_PORTX_DC       PORTD
#define SET_TFT_DC_HI      SET_BIT(TFT_PORTX_DC, TFT_DC_PIN);
#define SET_TFT_DC_LOW     RESET_BIT(TFT_PORTX_DC, TFT_DC_PIN);

#define TFT_RES_PIN         PD3
#define TFT_DDRX_RES        DDRD
#define TFT_PORTX_RES       PORTD
#define SET_TFT_RES_HI      SET_BIT(TFT_PORTX_RES, TFT_RES_PIN);
#define SET_TFT_RES_LOW     RESET_BIT(TFT_PORTX_RES, TFT_RES_PIN);




//#define TFT_SPCR (0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0)
/* Iterrupt: disable;
 * SPI: enable;
 * Data order: MSB;
 * Master/slave: Master;
 * Clock polarity: Rising;
 * Clock [hase: Leading -> Tralling edge;
 * Clock Rate: fclk/4 */


// free MCU from toggling CS GPIO
// Set this to 0 if not only one TFT is slave on that SPI
#define TFT_CS_ALWAS_ACTIVE 1
                        
#if TFT_CS_ALWAS_ACTIVE
  #undef SET_TFT_CS_HI
  #undef SET_TFT_CS_LOW
  #define SET_TFT_CS_HI
  #define SET_TFT_CS_LOW
  // make SET_TFT_CS_LOW; always active
  #define GRAB_TFT_CS     RESET_BIT(TFT_PORTX_CS, TFT_CS_PIN);
#endif


#define ENABLE_CMD()  SET_TFT_DC_LOW  \
                      SET_TFT_CS_LOW

#define ENABLE_DATA() SET_TFT_DC_HI   \
                      SET_TFT_CS_LOW
                      
#define RELEASE_TFT() SET_TFT_CS_HI

#define DISABLE_DATA() SET_TFT_CS_HI
                        
                        
extern uint8_t tabcolor, colstart, rowstart;
extern int16_t _width, _height;

//-------------------------------------------------------------------------------------------//

static const uint8_t Bcmd[] PROGMEM = {                  // Initialization commands for 7735B screens
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

static const uint8_t Rcmd1[] PROGMEM = {                 // Init for 7735R, part 1 (red or green tab)
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

static const uint8_t Rcmd2green[] PROGMEM = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

static const uint8_t Rcmd2red[] PROGMEM = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159

static const uint8_t Rcmd2green144[] PROGMEM = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F };           //     XEND = 127

static const uint8_t Rcmd3[] PROGMEM = {                 // Init for 7735R, part 3 (red or green tab)
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

static const uint8_t Gcmd[] PROGMEM = {                  // Initialization commands for 7735B screens
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

//-------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C"{
#endif

void writeCommand(uint8_t c);
void writeData(uint8_t d);
void writeWordData(uint16_t c);
  
void commandList(const uint8_t *addr);

void initB(void);                  // for ST7735B displays
void initR(uint8_t options);       // for ST7735R
void initG(void);                  // for ST7735B displays
void commonInit(const uint8_t *cmdList);

void tftSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void tftSetVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1);
void tftSetHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1);
void tftSetAddrPixel(uint16_t x0, uint16_t y0);
  
void tftSetScrollArea(uint16_t TFA, uint16_t BFA);
void tftScrollAddress(uint16_t VSP);
void tftScroll(uint16_t lines, uint16_t yStart);
void tftScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);

void tftSetRotation(uint8_t m);
void tftSetSleep(bool enable);
void tftSetIdleMode(bool mode);
void tftSetDispBrightness(uint8_t brightness);
void tftSetInvertion(bool i);   
//void setGamma(uint8_t gamma);
  
#define pushColorFast(a)  sendData16_SPI1(a)
  
#ifdef __cplusplus
} // extern "C"
#endif

#endif  /* _ST7735_H */

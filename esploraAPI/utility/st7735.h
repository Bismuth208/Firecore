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

extern const uint8_t Bcmd[];
extern const uint8_t Rcmd1[];
extern const uint8_t RcmdBlack[];
extern const uint8_t Rcmd2green[];
extern const uint8_t Rcmd2red[];
extern const uint8_t Rcmd2green144[];
extern const uint8_t Rcmd3[];
extern const uint8_t Gcmd[];

//-------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C"{
#endif

void writeCommand(uint8_t c);
void writeData(uint8_t d);
void writeWordData(uint16_t c);
  
void commandList(const uint8_t *addr);
void commonInit(const uint8_t *cmdList);

void initB(void);                  // for ST7735B displays
void initR(uint8_t options);       // for ST7735R
void initG(void);                  // for ST7735B displays
void initRBlack(void);

void tftSetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void tftSetVAddrWindow(uint8_t x0, uint8_t y0, uint8_t y1);
void tftSetHAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1);
void tftSetAddrPixel(uint8_t x0, uint8_t y0);
  
void tftSetScrollArea(uint8_t TFA, uint8_t BFA);
void tftScrollAddress(uint8_t VSP);
void tftScroll(uint8_t lines, uint8_t yStart);
void tftScrollSmooth(uint8_t lines, uint8_t yStart, uint8_t wait);

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

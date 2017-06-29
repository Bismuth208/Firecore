#ifndef _GFX_H
#define _GFX_H

#include <stdbool.h>
#include <stdint.h>

#include "st7735.h"


// Color definitions
#define COLOR_BLACK       0x0000      //   0,   0,   0
#define COLOR_NAVY        0x000F      //   0,   0, 128
#define COLOR_DARKGREEN   0x03E0      //   0, 128,   0
#define COLOR_DARKCYAN    0x03EF      //   0, 128, 128
#define COLOR_MAROON      0x7800      // 128,   0,   0
#define COLOR_PURPLE      0x780F      // 128,   0, 128
#define COLOR_OLIVE       0x7BE0      // 128, 128,   0
#define COLOR_DARKGREY    0x7BEF      // 128, 128, 128
#define COLOR_LIGHTGREY   0xC618      // 192, 192, 192
#define COLOR_BLUE        0x001F      //   0,   0, 255
#define COLOR_GREEN       0x07E0      //   0, 255,   0
#define COLOR_CYAN        0x07FF      //   0, 255, 255
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_RED         0xF800      // 255,   0,   0
#define COLOR_MAGENTA     0xF81F      // 255,   0, 255
#define COLOR_ORANGE      0xFD20      // 255, 165,   0
#define COLOR_YELLOW      0xFFE0      // 255, 255,   0
#define COLOR_WHITE       0xFFFF      // 255, 255, 255

//-------------------------------------------------------------------------------------------//
#ifdef __cplusplus
extern "C"{
#endif

void tftPushColor(uint16_t color);
void tftDrawPixel(int16_t x, int16_t y, uint16_t color);
void tftFillScreen(uint16_t color);
void tftFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void tftDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void tftDrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void tftFillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void tftDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void tftDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void tftDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

void tftDrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void tftDrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void tftFillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void tftFillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void tftDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void tftFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void tftDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void tftDrawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
void tftDrawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

uint16_t tftColor565(uint8_t r, uint8_t g, uint8_t b);
uint16_t tftConv8to16(uint8_t x);
  
int16_t tftHeight(void);
int16_t tftWidth(void);
    
uint16_t tftColumns(void);
uint16_t tftRows(void);

//uint8_t tftGetRotation(void);

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
int16_t tftGetCursorX(void);
int16_t tftGetCursorY(void);
  
//void setTextFont(unsigned char* f);
void tftDrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void tftSetCursor(int16_t x, int16_t y);
void tftSetTextColor(uint16_t c);
void tftSetTextColorBG(uint16_t c, uint16_t b);
void tftSetTextSize(int8_t s);
void tftSetTextWrap(bool w);
void tftSetCP437(bool x);

void tftPrint(const char *str);
void tftPrint_P(const char *str);
void tftPrintAt(int16_t x, int16_t y, const char *str);
void tftPrintAt_P(int16_t x, int16_t y, const char *str);
void tftPrintChar(uint8_t c);
void tftPrintCharAt(int16_t x, int16_t y, uint8_t c);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _GFX_H */

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <avr/pgmspace.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <systicktimer.h>
#include <spi.h>

#include "gfx.h"
#include "glcdfont.h"

//-------------------------------------------------------------------------------------------//

#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif

#ifndef sign
#define sign(x) ((x)>0?(1):-(1))
#endif

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

#ifndef swap
#define swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

//-------------------------------------------------------------------------------------------//

static uint8_t textsize = 1;
//static uint8_t rotation = 0;

static uint16_t textcolor   = COLOR_WHITE;
static uint16_t textbgcolor = COLOR_WHITE;

static int16_t cursor_x = 0;
static int16_t cursor_y = 0;

static bool wrap = true;   // If set, 'wrap' text at right edge of display
static bool _cp437 = false; // If set, use correct CP437 charset (default is off)

//-------------------------------------------------------------------------------------------//

void tftDrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t x = -r, y = 0, err = 2-2*r, e2;
  
  do {
    tftDrawPixel(x0-x, y0+y, color);
    tftDrawPixel(x0+x, y0+y, color);
    tftDrawPixel(x0+x, y0-y, color);
    tftDrawPixel(x0-x, y0-y, color);
    
    e2 = err;
    if (e2 <= y) {
      err += ++y*2+1;
      if ((-x == y) && (e2 <= x)) e2 = 0;
    }
    if (e2 > x) err += ++x*2+1;
  } while (x <= 0);
}

void tftFillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  tftDrawFastVLine(x0, y0-r, 2*r+1, color);
  tftFillCircleHelper(x0, y0, r, 3, 0, color);
}

void tftDrawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      tftDrawPixel(x0 + x, y0 + y, color);
      tftDrawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      tftDrawPixel(x0 + x, y0 - y, color);
      tftDrawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      tftDrawPixel(x0 - y, y0 + x, color);
      tftDrawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      tftDrawPixel(x0 - y, y0 - x, color);
      tftDrawPixel(x0 - x, y0 - y, color);
    }
  }
}

// Used to do circles and roundrects
void tftFillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    
    if (cornername & 0x1) {
      tftDrawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      tftDrawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      tftDrawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      tftDrawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

// Bresenham's algorithm
void tftDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  if (y0 == y1) {
    if (x1 > x0) {
      tftDrawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else if (x1 < x0) {
      tftDrawFastHLine(x1, y0, x0 - x1 + 1, color);
    } else {
      tftDrawPixel(x0, y0, color);
    }
    return;
  } else if (x0 == x1) {
    if (y1 > y0) {
      tftDrawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else {
      tftDrawFastVLine(x0, y1, y0 - y1 + 1, color);
    }
    return;
  }
  
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  
  int16_t dx = x1 - x0;
  int16_t dy = abs(y1 - y0);
  
  int16_t err = dx / 2;
  int16_t ystep;
  
  int16_t xbegin = x0;
  int16_t len;
  
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
    
  if (steep) {
    for (; x0<=x1; x0++) {
      err -= dy;
      if (err < 0) {
        len = x0 - xbegin;
        if (len) {
          tftDrawFastVLine(y0, xbegin, len + 1, color);
        } else {
          tftDrawPixel(y0, x0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      tftDrawFastVLine(y0, xbegin, x0 - xbegin, color);
    }
    
  } else {
    for (; x0<=x1; x0++) {
      err -= dy;
      if (err < 0) {
        len = x0 - xbegin;
        if (len) {
          tftDrawFastHLine(xbegin, y0, len + 1, color);
        } else {
          tftDrawPixel(x0, y0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      tftDrawFastHLine(xbegin, y0, x0 - xbegin, color);
    }
  }
  
}

// Draw a rectangle
void tftDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  tftDrawFastHLine(x, y, w, color);
  tftDrawFastHLine(x, y+h-1, w, color);
  tftDrawFastVLine(x, y, h, color);
  tftDrawFastVLine(x+w-1, y, h, color);
}

// Draw a rounded rectangle
void tftDrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  tftDrawFastHLine(x+r  , y    , w-2*r, color); // Top
  tftDrawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  tftDrawFastVLine(x    , y+r  , h-2*r, color); // Left
  tftDrawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  tftDrawCircleHelper(x+r    , y+r    , r, 1, color);
  tftDrawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  tftDrawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  tftDrawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void tftFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  tftFillRect(x+r, y, w-2*r, h, color);
  
  // draw four corners
  tftFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  tftFillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void tftDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  tftDrawLine(x0, y0, x1, y1, color);
  tftDrawLine(x1, y1, x2, y2, color);
  tftDrawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void tftFillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, y, last;
  
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  
  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    tftDrawFastHLine(a, y0, b-a+1, color);
    return;
  }
  
  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;
  
  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it
  
  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    tftDrawFastHLine(a, y, b-a+1, color);
  }
  
  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    tftDrawFastHLine(a, y, b-a+1, color);
  }
}

void tftDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tftDrawPixel(x+i, y+j, color);
      }
    }
  }
}

// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
void tftDrawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        tftDrawPixel(x+i, y+j, color);
      } else {
      	tftDrawPixel(x+i, y+j, bg);
      }
    }
  }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void tftDrawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        tftDrawPixel(x+i, y+j, color);
      }
    }
  }
}

//=================================== Print section ========================//
void tftPrint(const char *str)
{
  uint16_t size = strlen(str);
  
  while (size--) {
    tftPrintChar(*str++);
  }
}

void tftPrint_P(const char *str)
{
  for(uint16_t count=0; count < strlen_P(str); count++) {
    tftPrintChar(pgm_read_byte(str + count));
  }
}

void tftPrintCharAt(int16_t x, int16_t y, uint8_t c)
{
  tftSetCursor(x, y);
  tftPrintChar(c);
}

void tftPrintChar(uint8_t c)
{
  switch(c) {
    case '\n': {
      cursor_y += textsize*8;
      cursor_x  = 0;
    } break;
      
    case '\r': {
      cursor_x  = 0;
    } break;
      
    default: {
      tftDrawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
      cursor_x += textsize*6;
      if (wrap && (cursor_y > (_height - textsize*8))) {
        cursor_y = 0;
        cursor_x += textsize*5;
      }
      if (wrap && (cursor_x > (_width - textsize*6))) {
        cursor_y += textsize*8;
        cursor_x = 0;
      }
    } break;
  }
}

void tftDrawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{
  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
         ((y + 8 * size - 1) < 0))   // Clip top
    return;
  
  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior
  
  uint8_t line;
  int8_t i, j;
  
  
  if(size == 1) {
    for (i=0; i<6; i++ ) {
      
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(font+(c*5)+i);
      
      for (j = 0; j<8; j++) {
        if (line & 0x1) tftDrawPixel(x+i, y+j, color);
        else if (bg != color) {
          tftDrawPixel(x+i, y+j, bg);
        }
        line >>= 1;
      }
    }
  } else {
    for (i=0; i<6; i++ ) {
      
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(font+(c*5)+i);
      
      for (j = 0; j<8; j++) {
        if (line & 0x1) {
          tftFillRect(x+(i*size), y+(j*size), size, size, color);
        } else if (bg != color) {
          tftFillRect(x+i*size, y+j*size, size, size, bg);
        }
        line >>= 1;
      }
    }
  }
}

uint16_t tftColumns()
{
  return (tftWidth() / (textsize*6));
}

uint16_t tftRows()
{
  return (tftHeight() / (textsize*8));
}
/*
void tftSetFont(unsigned char* f)
{
font = f;
}
*/
void tftSetCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

int16_t tftGetCursorX(void)
{
  return cursor_x;
}

int16_t tftGetCursorY(void)
{
  return cursor_y;
}

void tftSetTextSize(int8_t s)
{
  textsize = (s > 0) ? s : 1;
}

void tftSetTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void tftSetTextColorBG(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b; 
}

void tftSetTextWrap(bool w)
{
  wrap = w;
}
//==========================================================================//

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void tftSetCP437(bool x)
{
  _cp437 = x;
}

// Return the size of the display (per current rotation)
int16_t tftWidth(void)
{
  return _width;
}

int16_t tftHeight(void)
{
  return _height;
}

/***************************************************************************/

void tftPushColor(uint16_t color)
{
  sendData16_SPI1(color);
}

void tftDrawPixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
  
  tftSetAddrPixel(x, y);

  sendData16_SPI1(color);
  RELEASE_TFT();
}

void tftFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;
  
  tftSetAddrWindow(x, y, x+w-1, y+h-1);
  
  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      sendData16_SPI1(color);
    }
  }
  RELEASE_TFT();
}

void tftDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  
  tftSetVAddrWindow(x, y, y+h-1);
  
  while (h--) {
    sendData16_SPI1(color);
  }
  RELEASE_TFT();
}

void tftDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  
  tftSetHAddrWindow(x, y, x+w-1);
  
  while (w--) {
    sendData16_SPI1(color);
  }
  RELEASE_TFT();
}

void tftFillScreen(uint16_t color)
{
  tftFillRect(0, 0,  _width, _height, color);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t tftColor565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// converts rgb 332 to rgb 565
uint16_t tftConv8to16(uint8_t x)
{
  uint16_t r, g, b;
  r = x>>5;
  g = (x&0x1c)>2;
  b = x&0x03;
  return (((r<<13) + (r<<10))&0xf800) + (g<<8) + (g<<5) + (b<<3) + (b<<1) + (b>>1);
}

/*
uint8_t tftGetRotation(void)
{
return rotation;
}
*/

#ifndef _PICS_H
#define _PICS_H

#include <avr/pgmspace.h>  // for progmem

//---------------------------------------------------------------------------//

#define getPicByte(a)     pgm_read_byte(a)
#define getPicWord(a, b)  pgm_read_word(&(a[b]))
//---------------------------------------------------------------------------//

// Color definitions
#define BACKGROUND_COLOR  0x01F4      //   1,  51, 102  // 0x013466

#define INDIGO_COLOR   tftColor565(65, 3, 140)

#define RNDCLR(a ,b) a, b, (RN % 255), (((RN % 192 + 64) & 0xFC) << 3), COLOR_BLACK, 1

//---------------------------------------------------------------------------//
/*
 * Palette based on Typical Famicom NES (Dendy) Palette.
 * Consist of 80 colors (actually 75; 5 transparent colors 0x0F-0x4F).
 *
 * 8 Gray colors 0x0D-0x3D; 0x1E-0x4E
 *
 * 0x0E pure black.
 * 0x4D pure white.
 * Suggest to use 0x0F-0x4F section for transparent.
 *
 * Colors represented in RGB565 color space.
 *
 * Color adress: 0x00 - 0x4F
 */

//Size: 160 ( 80 * sizeof(uint16_t) )
extern const uint16_t nesPalette_ext[];
//---------------------------------------------------------------------------//

#define ROCKET_W        13
#define ROCKET_H         7
#define ROCKET_PIC_SIZE 62

// orig size 91 * 2 = 182
// RLE compress ratio 2.93548
extern const uint8_t rocketPic[];
//---------------------------------------------------------------------------//

#define DOGE_PIC_W     50
#define DOGE_PIC_H     50
#define DOGE_PIC_SIZE  976

// orig size 2500 * 2 = 5000
// RLE compress ratio 5.12295
extern const uint8_t cityDogePic[];
//---------------------------------------------------------------------------//

#define GALAXY_PIC_W     160
#define GALAXY_PIC_H     70
#define GALAXY_PIC_SIZE  6957

// orig size 11200 * 2 = 22400
// RLE compress ratio 3.21978
extern const uint8_t galaxyPic[];
//---------------------------------------------------------------------------//

#define ALIEN_ROCKET_PIC_W     10
#define ALIEN_ROCKET_PIC_H      5
#define ALIEN_ROCKET_PIC_SIZE  36

// orig size 50 * 2 = 100
// RLE compress ratio 2.77778
extern const uint8_t alienRocketPic[];


#define ALIEN_SHIP_PIC_W   32
#define ALIEN_SHIP_PIC_H   16

// orig size 512 * 2 = 1024
// RLE compress ratio 5.17172
#define ALIEN_SHIP_HI_PIC_SIZE 198
extern const uint8_t alienShipHi[];

// orig size 512 * 2 = 1024
// RLE compress ratio 5.22449
#define ALIEN_SHIP_LOW_PIC_SIZE 196
extern const uint8_t alienShipLow[];

#define ALIEN_SHIP_BOSS_PIC_W 22
#define ALIEN_SHIP_BOSS_PIC_H 30
#define ALIEN_SHIP_BOSS_PIC_SIZE 310
// orig size 660 * 2 = 1320
// RLE compress ratio 4.25806
extern const uint8_t bossShip[];
//---------------------------------------------------------------------------//

#define REAPER_PIC_W     50
#define REAPER_PIC_H     50
#define REAPER_PIC_SIZE  1021

// Picture compressed by RLE
extern const uint8_t reapersCity[];
//---------------------------------------------------------------------------//

#define SHIP_PIC_W   32
#define SHIP_PIC_H   18
#define SHIP_BASE_HI_PIC_SIZE 264

// orig size 576 * 2 = 1152
// RLE compress ratio 4.36364
extern const uint8_t shipBaseHi[];

// orig size 576 * 2 = 1152
// RLE compress ratio 4.28253
#define SHIP_BASE_LOW_PIC_SIZE 269
extern const uint8_t shipBaseLow[];
//---------------------------------------------------------------------------//

#define TEXT_TITLE_HI_W    115
#define TEXT_TITLE_HI_H    18
#define TEXT_TITLE_HI_SIZE 799

// Picture compressed by RLE
extern const uint8_t titleTextPic[];
//---------------------------------------------------------------------------//

#define PIC_TITLE_ROW_WH      16
#define ROWS_L_DATA_PIC_SIZE  74
#define ROWS_R_DATA_PIC_SIZE  75

// orig size 256 * 2 = 512
// RLE compress ratio 6.91892
extern const uint8_t rowsLeftPic[];

// orig size 256 * 2 = 512
// RLE compress ratio 6.82667
extern const uint8_t rowsRightPic[];
//---------------------------------------------------------------------------//

#endif  

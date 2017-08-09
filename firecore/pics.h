/*
 * Author: Antonov Alexandr (Bismuth208)
 * e-mail: bismuth20883@gmail.com
 * 
 *  THIS PROJECT IS PROVIDED FOR EDUCATION/HOBBY USE ONLY
 *  NO PROTION OF THIS WORK CAN BE USED IN COMMERIAL
 *  APPLICATION WITHOUT WRITTEN PERMISSION FROM THE AUTHOR
 *  
 *  EVERYONE IS FREE TO POST/PUBLISH THIS ARTICLE IN
 *  PRINTED OR ELECTRONIC FORM IN FREE/PAID WEBSITES/MAGAZINES/BOOKS
 *  IF PROPER CREDIT TO ORIGINAL AUTHOR IS MENTIONED WITH LINKS TO
 *  ORIGINAL ARTICLE
 */

#ifndef _PICS_H
#define _PICS_H

#include <avr/pgmspace.h>

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
 * Palette based on Typical Famicom\NES (Dendy) Palette.
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

// size: 160 ( 80 * sizeof(uint16_t) )
extern const uint16_t nesPalette_ext[];
//---------------------------------------------------------------------------//

#define LASER_PIC_W 16
#define LASER_PIC_H 8

// orig size 128 * 2 = 256
// RLE compress ratio 14.2222
extern const uint8_t weaponLaserPic1[];

// orig size 128 * 2 = 256
// RLE compress ratio 7.52941
extern const uint8_t weaponLaserPic2[];

// orig size 128 * 2 = 256
// RLE compress ratio 5.22449
extern const uint8_t weaponLaserPic3[];

// orig size 128 * 2 = 256
// RLE compress ratio 3.93846
extern const uint8_t weaponLaserPic4[];

// orig size 128 * 2 = 256
// RLE compress ratio 8
extern const uint8_t weaponLaserPic5[];

extern const uint8_t * const laserPics[];
//---------------------------------------------------------------------------//

#define GIFT_PIC_W 16
#define GIFT_PIC_H 16

// orig size 256 * 2 = 512
// RLE compress ratio 5.56522
extern const uint8_t giftHeartPic[];

// orig size 256 * 2 = 512
// RLE compress ratio 4.97087
extern const uint8_t giftWeaponPic[];
//---------------------------------------------------------------------------//

#define HUD_GUI_PIC_W   128
#define HUD_GUI_PIC_H     8

// orig size 1024 * 2 = 2048
// RLE compress ratio 31.5077
extern const uint8_t hudGuiPic[];
//---------------------------------------------------------------------------//

#define DOGE_PIC_W     50
#define DOGE_PIC_H     50

// orig size 2500 * 2 = 5000
// RLE compress ratio 5.24109
extern const uint8_t cityDogePic[];
//---------------------------------------------------------------------------//

#define CHARACTER_ICON_W 32
#define CHARACTER_ICON_H 32

#define CHARACTER_FRAME_WH 32

#define CHARACTER_ICON_OFFSET_X 5
#define CHARACTER_ICON_OFFSET_Y 8

#define CHARACTER_ICON_STEP 50
#define CHARACTER_ICON_NUM   3

extern const uint8_t chiiCatPic[];
extern const uint8_t groompyCatPic[];
extern const uint8_t pushenCatPic[];
extern const uint8_t bonusCatPic[];

extern const uint8_t * const catsPics[];
//---------------------------------------------------------------------------//

#define GALAXY_PIC_W     160
#define GALAXY_PIC_H     70

// orig size 11200 * 2 = 22400
// RLE compress ratio 5.9448
extern const uint8_t galaxyPic[];
//---------------------------------------------------------------------------//

#define DEATHRAY_BOSS_PIC_W 16
#define DEATHRAY_BOSS_PIC_H 8
// orig size 128 * 2 = 256
// RLE compress ratio 3.76471
extern const uint8_t deathRayBossPic[];


#define DEATHRAY_PIC_W 8
#define DEATHRAY_PIC_H 8

// orig size 64 * 2 = 128
// RLE compress ratio 2.37037
extern const uint8_t deathRayHiPic[];

// orig size 64 * 2 = 128
// RLE compress ratio 3.36842
extern const uint8_t deathRayLowPic[];
//---------------------------------------------------------------------------//

#define ALIEN_SHIP_PIC_W   32
#define ALIEN_SHIP_PIC_H   16

// orig size 512 * 2 = 1024
// RLE compress ratio 5.17172
extern const uint8_t alienShipHi[];

// orig size 512 * 2 = 1024
// RLE compress ratio 5.22449
extern const uint8_t alienShipLow[];

#define ALIEN_SHIP_BOSS_PIC_W 22
#define ALIEN_SHIP_BOSS_PIC_H 30
// orig size 660 * 2 = 1320
// RLE compress ratio 4.25806
extern const uint8_t bossShip[];
//---------------------------------------------------------------------------//

#define SHIP_PIC_W   32
#define SHIP_PIC_H   16

// orig size 512 * 2 = 1024
// RLE compress ratio 5.33333
extern const uint8_t shipVSpeedPic[];

// orig size 512 * 2 = 1024
// RLE compress ratio 5.95349
extern const uint8_t shipVPowerPic[];

// orig size 512 * 2 = 1024
// RLE compress ratio 3.87879
extern const uint8_t shipVArmorPic[];

// orig size 512 * 2 = 1024
// RLE compress ratio 4.59193
extern const uint8_t shipBounsPic[];

extern const uint8_t * const shipsPics[];
//---------------------------------------------------------------------------//

#define FLAMES_PIC_W 6
#define FLAMES_PIC_H 4

// orig size 24 * 2 = 48
// RLE compress ratio 2
extern const uint8_t flameFireHiPic[];

// orig size 24 * 2 = 48
// RLE compress ratio 2
extern const uint8_t flameFireLowPic[];
//---------------------------------------------------------------------------//

#define TEXT_TITLE_HI_W    115
#define TEXT_TITLE_HI_H    18

// Picture compressed by RLE
extern const uint8_t titleTextPic[];
//---------------------------------------------------------------------------//

#define PIC_TITLE_ROW_WH      16

// orig size 256 * 2 = 512
// RLE compress ratio 6.91892
extern const uint8_t rowsLeftPic[];

// orig size 256 * 2 = 512
// RLE compress ratio 6.82667
extern const uint8_t rowsRightPic[];
//---------------------------------------------------------------------------//

#endif  

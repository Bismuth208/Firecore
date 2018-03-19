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
#define getPicSize(a, b)  wordData_t{.wData = getPicWord(a, b)}
//---------------------------------------------------------------------------//

// Color definitions
#define BACKGROUND_COLOR  0x01F4      //   1,  51, 102  // 0x013466

#define INDIGO_COLOR   tftColor565(65, 3, 140)
#define TXTRNCLR (((RN % 192 + 64) & 0xFC) << 3), COLOR_BLACK
#define RNDCLR(a ,b) a, b, (RN % 255)

//---------------------------------------------------------------------------//
/*
 * Palette based on typical old-scool 8bit graphics.
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

 #define PALETTE_SIZE 160

// size: 160 ( 80 * sizeof(uint16_t) )
extern const uint16_t palette_ext[];
extern uint16_t palette_RAM[];
//---------------------------------------------------------------------------//

extern uint16_t currentBackGroundColor;
extern uint8_t currentBackGroundColorId;
extern uint8_t replaceColorId;
extern const uint8_t lvlColors[];
//---------------------------------------------------------------------------//

#define LASER_PIC_W 16
#define LASER_PIC_H 8

extern const uint8_t weaponLaserPic1[];
extern const uint8_t weaponLaserPic2[];
extern const uint8_t weaponLaserPic3[];
extern const uint8_t weaponLaserPic4[];
extern const uint8_t weaponLaserPic5[];

extern const uint8_t * const laserPics[];
//---------------------------------------------------------------------------//

#define GIFT_PIC_W 16
#define GIFT_PIC_H 16

extern const uint8_t giftHeartPic[];
extern const uint8_t giftWeaponPic[];
//---------------------------------------------------------------------------//

#define HUD_GUI_PIC_W   128
#define HUD_GUI_PIC_H     8

extern const uint8_t hudGuiPic[];
//---------------------------------------------------------------------------//

#define TEXT_WINDOW_PIC_W   160
#define TEXT_WINDOW_PIC_H    40

extern const uint8_t textWindowPic[];
//---------------------------------------------------------------------------//

#define DOGE_PIC_W     50
#define DOGE_PIC_H     50

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

extern const uint8_t deathRayBossPic[];


#define DEATHRAY_PIC_W 8
#define DEATHRAY_PIC_H 8

extern const uint8_t deathRayHiPic[];
extern const uint8_t deathRayLowPic[];
//---------------------------------------------------------------------------//

#define ALIEN_SHIP_FLAME_WH 4

extern const uint8_t alienShipFireHi[];
extern const uint8_t alienShipFireLow[];


#define ALIEN_SHIP_PIC_W   32
#define ALIEN_SHIP_PIC_H   16

extern const uint8_t alienShipV2[];

#define ALIEN_SHIP_BOSS_PIC_W 22
#define ALIEN_SHIP_BOSS_PIC_H 30

extern const uint8_t bossShip[];
//---------------------------------------------------------------------------//

#define SHIP_PIC_W   32
#define SHIP_PIC_H   16

extern const uint8_t shipVSpeedPic[];
extern const uint8_t shipVPowerPic[];
extern const uint8_t shipVArmorPic[];
extern const uint8_t shipBounsPic[];

extern const uint8_t * const shipsPics[];
//---------------------------------------------------------------------------//

#define FLAMES_PIC_W 6
#define FLAMES_PIC_H 4

extern const uint8_t flameFireHiPic[];
extern const uint8_t flameFireLowPic[];
//---------------------------------------------------------------------------//

#define TEXT_TITLE_HI_W    115
#define TEXT_TITLE_HI_H    18

extern const uint8_t titleTextPic[];
//---------------------------------------------------------------------------//

#define PIC_TITLE_ROW_WH      16

extern const uint8_t rowsLeftPic[];
extern const uint8_t rowsRightPic[];
//---------------------------------------------------------------------------//

#define EXPLOSION_PIC_WH 10

extern const uint8_t explosion1[];
extern const uint8_t explosion2[];
//---------------------------------------------------------------------------//

#endif /*_PICS_H*/ 

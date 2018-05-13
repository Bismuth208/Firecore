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

#include <esploraAPI.h>
#include "types.h"

//---------------------------------------------------------------------------//

// Color definitions
#define BACKGROUND_COLOR  0x01F4      //   1,  51, 102  // 0x013466

#define INDIGO_COLOR   tftColor565(65, 3, 140)
#define TXTRNCLR (((RN % 192 + 64) & 0xFC) << 3), COLOR_BLACK
#define RNDCLR(a ,b) a, b, (RN % 255)
//---------------------------------------------------------------------------//

extern uint16_t currentBackGroundColor;
extern pic_t lvlColors[];
//---------------------------------------------------------------------------//

#define LASER_PIC_W 16
#define LASER_PIC_H 8

extern pic_t* const laserPics[];
//---------------------------------------------------------------------------//

#define GIFT_PIC_W 16
#define GIFT_PIC_H 16

extern pic_t giftHeartPic[];
extern pic_t giftWeaponPic[];
//---------------------------------------------------------------------------//

#define HUD_GUI_PIC_W   128
#define HUD_GUI_PIC_H     8

extern pic_t hudGuiPic[];
//---------------------------------------------------------------------------//

#define TEXT_WINDOW_PIC_W   160
#define TEXT_WINDOW_PIC_H    40

extern pic_t textWindowPic[];
//---------------------------------------------------------------------------//

#define DOGE_PIC_W     50
#define DOGE_PIC_H     50

extern pic_t cityDogePic[];
//---------------------------------------------------------------------------//

#define CHARACTER_ICON_W 32
#define CHARACTER_ICON_H 32

#define CHARACTER_FRAME_WH 32

#define CHARACTER_ICON_OFFSET_X 5
#define CHARACTER_ICON_OFFSET_Y 8

#define CHARACTER_ICON_STEP 50
#define CHARACTER_ICON_NUM   3

extern pic_t * const catsPics[];
//---------------------------------------------------------------------------//

#define GALAXY_PIC_W     160
#define GALAXY_PIC_H     70

// orig size 11200 * 2 = 22400
// RLE compress ratio 5.9448
extern pic_t galaxyPic[];
//---------------------------------------------------------------------------//

#define DEATHRAY_BOSS_PIC_W 16
#define DEATHRAY_BOSS_PIC_H 8

extern pic_t deathRayBossPic[];


#define DEATHRAY_PIC_W 8
#define DEATHRAY_PIC_H 8

extern pic_t deathRayHiPic[];
extern pic_t deathRayLowPic[];
//---------------------------------------------------------------------------//

#define ALIEN_SHIP_FLAME_WH 4

extern pic_t alienShipFireHi[];
extern pic_t alienShipFireLow[];


#define ALIEN_SHIP_PIC_W   32
#define ALIEN_SHIP_PIC_H   16

extern pic_t alienShipV2[];

#define ALIEN_SHIP_BOSS_PIC_W 22
#define ALIEN_SHIP_BOSS_PIC_H 28

extern pic_t bossShip[];
extern pic_t ship_boss_v2[];
// extern pic_t * const bossShipPics[];
//---------------------------------------------------------------------------//

#define SHIP_PIC_W   32
#define SHIP_PIC_H   16

extern pic_t * const shipsPics[];
//---------------------------------------------------------------------------//

#define FLAMES_PIC_W 6
#define FLAMES_PIC_H 4

extern pic_t flameFireHiPic[];
extern pic_t flameFireLowPic[];
//---------------------------------------------------------------------------//

#define TEXT_TITLE_HI_W    115
#define TEXT_TITLE_HI_H    18

extern pic_t titleTextPic[];
//---------------------------------------------------------------------------//

#define PIC_TITLE_ROW_WH      16

extern pic_t rowsLeftPic[];
extern pic_t rowsRightPic[];
//---------------------------------------------------------------------------//

#define EXPLOSION_PIC_WH 10

extern pic_t explosion1[];
extern pic_t explosion2[];
//---------------------------------------------------------------------------//


#define ASTEROID_SM_PIC_WH 8
#define ASTEROID_L_PIC_WH  10

extern pic_t * const asteroidsPics[];
//---------------------------------------------------------------------------//

#define CREDIT_PHOTO_WH 50

extern pic_t creditPicQR[];
extern pic_t creditPicOne[];
extern pic_t creditPicTwo[];
//---------------------------------------------------------------------------//

#endif /*_PICS_H*/ 

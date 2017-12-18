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

#ifndef _TYPES_H
#define _TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//
typedef struct { // 4 bytes RAM
  uint8_t x;
  uint8_t y;
} position_t;

typedef struct {  // need for collision check
  position_t Base;
  position_t New;
} objPosition_t;

typedef struct {
  position_t P0;
  position_t P1;
  position_t P2;
  uint8_t totalSteps;
} bezier_t;

typedef struct {
  uint8_t id;
  uint8_t step;
} bezierLine_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  uint8_t color; // as we use 8bit palette, store only color id
  uint8_t speed;
} stars_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  struct {
    uint8_t onUse   :1;
    uint8_t state   :1;
    uint8_t freeRam :6;
  };
} rocket_t;

typedef struct {
  rocket_t deathRay;
  uint16_t timeToShoot;
} deathRay_t;

typedef const uint8_t pic_t;

typedef struct {  // 8 bytes RAM
  objPosition_t pos;
  pic_t *pPic;
  bezierLine_t bezLine;
} gift_t;

typedef struct {
  //uint8_t bombsLeft;
  //uint8_t rocketsLeft;
  pic_t *pPic;
  struct {
    uint8_t overHeated;//    :1;
    uint8_t state;//         :1;
    //uint8_t bombsUnlocked :1;
    uint8_t level;//         :3;
    //uint8_t freeRam       :3;
  };
} weapon_t;

typedef struct {
  int8_t speed;
  int8_t power;
  int8_t durability;
} shipStats_t;

typedef struct {  // 8 + 5 + 5 bytes RAM
  objPosition_t pos;
  shipStats_t states;
  weapon_t weapon;
  pic_t *pBodyPic;
  int16_t health;
  uint8_t type;
  bool flameState;
} ship_t;

typedef struct {  // 8 + 6 + 5 bytes RAM
  objPosition_t pos;
  bezierLine_t bezLine;
  deathRay_t weapon;
  struct {
    uint8_t state;//   :1;     // Pic what we draw
    uint8_t alive;//   :1;
    //uint8_t freeRam :6;
  };
  int16_t health;
  uint16_t timeToShoot;
  uint8_t respawnTime;  // 255 sec will be enougth... If call checkInVadersRespawn 1 time in sec...
} inVader_t;

typedef struct {
  inVader_t base;
  deathRay_t weapons[4];
} inVaderBoss_t;

typedef struct {  // 1 byte RAM
  struct {
    uint8_t updLife;//  :1;
    uint8_t updScore;// :1;
    //uint8_t freeRam :6;
  };
} hudStatus_t;

typedef struct {
  union {
  	uint8_t zBtn; // for fast reset only
  	struct {
  	  uint8_t aBtn :1;
  	  uint8_t bBtn :1;
  	  uint8_t xBtn :1;
  	  uint8_t yBtn :1;
  	  uint8_t freeRam :4;
    };
  };
} btnStatus_t;

typedef struct {
  uint8_t name[4]; // 3 for name 4 for '\n'
  uint16_t score;  // 65535 score val be enought right?
} saveData_t;

typedef union {
  uint16_t wData;
  struct {
    uint8_t u8Data1;
    uint8_t u8Data2;
  };
} wordData_t;

//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/

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

#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//
#define MAX_PEW_PEW              4  // Crysis, nanosuit voice: - "Maximum pew".
#define ALIEN_BOSS_DEATH_RAYS    4

//---------------------------------------------------------------------------//
typedef struct {  // 4 bytes
  position_t pos;
  uint8_t color; // as we use 8bit palette, store only color id
  uint8_t speed;
} star_t;

typedef struct {
  sprite_t sprite;
  // position_t endpoint;
  uint8_t speed;
  struct {
    uint8_t onUse   :1;
    uint8_t type    :2;
    uint8_t angle   :2;
    uint8_t freeRam :3;
  };
} asteroid_t;

typedef struct {  // 7 bytes
  sprite_t sprite;
  struct {
    uint8_t onUse   :1;
    uint8_t freeRam :7;
  };
} rocket_t;

typedef struct {  // 9 bytes
  rocket_t ray;
  uint16_t timeToShoot;
} deathRay_t;

typedef struct {
  //uint8_t bombsLeft;
  //uint8_t rocketsLeft;
  rocket_t lasers[MAX_PEW_PEW];
  struct {
    //uint8_t overHeated;//    :1;
    //uint8_t state;//         :1;
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

//---------------------------------------------------------------------------//
typedef struct {  // 8 bytes
  sprite_t sprite;
  bezierLine_t bezLine;
} gift_t;

typedef struct {
  sprite_t sprite;
  shipStats_t states;
  weapon_t weapon;
  int16_t health;
  uint8_t type;
  bool flameState;
  bool lowHealthState;
} ship_t;

typedef struct {
  sprite_t sprite;
  bezierLine_t bezLine;
  deathRay_t weapon;
  struct {
    uint8_t alive   :1;
    uint8_t freeRam :7;
  };
  int16_t health;
  uint16_t timeToShoot;
  uint8_t respawnTime;  // 255 sec will be enougth... If call checkInVadersRespawn 1 time in sec...
} inVader_t;

typedef struct {
  inVader_t base;
  deathRay_t weapons[ALIEN_BOSS_DEATH_RAYS];
} inVaderBoss_t;

//---------------------------------------------------------------------------//

typedef struct {
  union {
    uint8_t rawData[8];
    struct {
      uint8_t name[4];        // 3 for name 4 for '\n'
      uint16_t score;         // 65535 score val be enought, i'm right?
      uint8_t bonusUnlocked;  // galaxy saved and 4th ship unlocked
      uint8_t saveDataMark;   // ...
    };
  };
} saveData_t;
//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_TYPES_H*/

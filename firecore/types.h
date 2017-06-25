#ifndef _TYPES_H
#define _TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//
typedef struct { // 4 bytes RAM
  int16_t x;
  int16_t y;
} position_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  uint8_t color; // as we use NES palette, store only color id
} stars_t;

typedef struct {  // 5 bytes RAM
  position_t pos;
  struct {
    uint8_t onUse   :1;
    uint8_t freeRam :7;
  };
} rocket_t;

typedef struct {  // 8 bytes RAM
  position_t posBase;
  position_t posNew;
  bool state;
} gift_t;

typedef struct {
  int8_t  bombsLeft;
  int16_t rocketsLeft;
  struct {
    uint8_t overHeated    :1;
    uint8_t state         :1;
    uint8_t bombsUnlocked :1;
    uint8_t freeRam       :5;
  };
} weapon_t;

typedef struct {  // 4 + 4 + 5 + 5 bytes RAM
  position_t posBase;
  position_t posNew;
  weapon_t weapon;
  int16_t health;
  int8_t speed;
  //int16_t power;
  //int16_t durability;
} ship_t;

typedef struct {  // 4 + 6 + 3 bytes RAM
  position_t posBase;
  position_t posNew;
  struct {
    uint8_t state   :1;     // Pic what we draw
    uint8_t alive   :1;
    uint8_t freeRam :6;
  };
  int16_t health;
  uint16_t timeToShoot;
  uint8_t respawnTime;  // 255 sec will be enougth... If call checkInVadersRespawn 1 time in sec...
  //uint8_t respawnCount;
  rocket_t deathRay;
} inVader_t;

typedef struct {  // 1 byte RAM
  struct {
    uint8_t updLife  :1;
    uint8_t updPew   :1;
    //uint8_t updScore     :1;
    uint8_t freeRam      :6;
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

//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/

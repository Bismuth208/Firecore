#include <avr/pgmspace.h> 

#include "common.h"

const uint16_t enemyShotPattern[] PROGMEM = {
  0x8005,0x884D,0x81C1,0x250,0x0000
};

const uint16_t enemyHitPattern[] PROGMEM = {
  0x8005,0x81C1,0x108,0x0000
};

const uint16_t enemyDestroyPattern[] PROGMEM = {
  0x8045,0x8851,0x8241,0x538,0x0000
};

const uint16_t playerDestroyPattern[] PROGMEM = {
  0x8045,0x8891,0x8241,0x608,0x0000
};

const uint16_t playerSuperPattern[] PROGMEM = {
  0x8005,0x81C1,0x164,0x164,0x164,0x0000
};

const uint16_t playerShotPattern[] PROGMEM = {
  0x8005,0x8141,0x14C,0x154,0x0000
};

const uint16_t beepPattern[] PROGMEM = {0x118,0x000};

#include "help.h"

uint8_t rndNum =1;
uint8_t rndNumA =0;

uint8_t fastRandNum(void)
{
  rndNum ^= (rndNum << 3);
  rndNum ^= (rndNum >> 5);
  rndNum ^= (rndNumA++ >> 2);
  return rndNum;
}

void seedRndNum(uint16_t seed)
{
  rndNum += seed>>8;
  rndNum += seed & 0x00FF;
}

int32_t mapVal(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// fast and low cost memset
void memset_F(void *pvDest, uint8_t src, size_t size)
{
  uint8_t *dest = (uint8_t*)pvDest;
  do {
    *dest++ = src;
  } while(--size);
}

void memcpy_F(void *pDest, void *pSrc, size_t size)
{
  uint8_t *puDest = (uint8_t*)pDest;
  uint8_t *puSrc = (uint8_t*)pSrc;
  
  do {
    *puDest++ = *puSrc++;
  } while(--size);
}

#ifndef _HELP_H
#define _HELP_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

uint8_t fastRandNum(void);
void seedRndNum(uint16_t seed);

int32_t mapVal(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
  
void memset_F(void *pvDest, uint8_t src, size_t size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_HELP_H*/

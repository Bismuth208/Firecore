#ifndef _RLEUNPACK_H
#define _RLEUNPACK_H

#include <avr/pgmspace.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void drawBMP_ERLE_P(uint8_t x, uint8_t y, const uint8_t *pPic);
void drawBMP_RLE_P(uint8_t x, uint8_t y, const uint8_t *pPic);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_RLEUNPACK_H*/

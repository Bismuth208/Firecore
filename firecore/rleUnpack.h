#ifndef _RLEUNPACK_H
#define _RLEUNPACK_H

#include <avr/pgmspace.h>
#include <stdint.h>

#include <esploraAPI.h>

#include "types.h"     // for wordData_t, pic_t
#include "pics.h"      // for palette_RAM[]

#ifdef __cplusplus
extern "C"{
#endif

void drawBMP_ERLE_P(uint8_t x, uint8_t y, pic_t *pPic);
void drawBMP_RLE_P(uint8_t x, uint8_t y, pic_t *pPic);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_RLEUNPACK_H*/

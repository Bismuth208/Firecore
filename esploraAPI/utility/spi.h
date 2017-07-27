#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void initSPI(void);

void sendData8_SPI1(uint8_t data);
void sendData16_SPI1(uint16_t data);
void sendArrSPI(uint8_t *buf, uint16_t size);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_SPI_H*/

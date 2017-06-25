#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>

#define SET_SCK_LOW     PORTB &= ~ (1 << PB1);
#define SET_MOSI_HI     PORTB |= (1 << PB2);
#define SET_SCK_HI      PORTB |= (1 << PB1);
#define SET_SS_HI       PORTB |= (1 << PB0);
/*
 PB2 - D10; SS
 PB3 - D11; MOSI
 PB4 - D12; MISO
 PB5 - D13; SCK
 */

#ifdef __cplusplus
extern "C"{
#endif

void initSPI(void);
void beginTrSPI(uint8_t spcr, uint8_t spsr);
void endTrSPI(void);
uint8_t sendReadDataSPI(uint8_t data);
uint16_t sendReadWordDataSPI(uint16_t data);
void sendData8_SPI1(uint8_t data);
void sendData16_SPI1(uint16_t data);
void sendReadArrSPI(void *buf, uint16_t count);
void sendArrSPI(uint8_t *buf, uint16_t size);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_SPI_H*/

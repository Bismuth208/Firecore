#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>

/*
 * The following NOP introduces a small delay that can prevent the wait
 * loop form iterating when running at the maximum speed. This gives
 * about 10% more speed, even if it seems counter-intuitive. At lower
 * speeds it is unnoticed.
 */
#define SPDR_TX_WAIT  asm volatile("nop"); while((SPSR & (1<<SPIF)) == 0);

typedef union {
  uint16_t val;
  struct {
    uint8_t lsb;
    uint8_t msb;
  };
} SPDR_t;    // this one will use only registers

#ifdef __cplusplus
extern "C"{
#endif

void initSPI(void);

void sendData8_SPI1(uint8_t data);
void sendData16_SPI1(uint16_t data);
void sendArrSPI(uint8_t *buf, uint16_t size);

void sendData8Dirt_SPI1(uint8_t bData, uint8_t len);
  
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_SPI_H*/

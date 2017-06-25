#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"

/*
 * The following NOP introduces a small delay that can prevent the wait
 * loop form iterating when running at the maximum speed. This gives
 * about 10% more speed, even if it seems counter-intuitive. At lower
 * speeds it is unnoticed.
 */
#define SPDR_TX_WAIT asm volatile("nop"); while (!(SPSR & (1<<SPIF))) ;

static uint8_t backupSPCR;
static uint8_t backupSPSR;
static uint8_t backupSREG;

typedef union {
    uint16_t val;
    struct {
        uint8_t lsb;
        uint8_t msb;
    };
} SPDR_t;    // this one will use only registers

void initSPI(void)
{
  backupSREG = SREG;
  cli();
  
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    
    /* PB2 - MOSI
     * PB1 - SCK
     * PB0 - CS (SS) // system chip select for the atmega board
     */

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.
  //SPCR =  (0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(0<<SPR1)|(0<<SPR0);
  SPCR = (1<<SPE) | (1<<MSTR);
  SPSR = (1 << SPI2X);  // double speed
    /* Iterrupt: disable;
     * SPI: enable;
     * Data order: MSB;
     * Master/slave: Master;
     * Clock polarity: Rising;
     * Clock [hase: Leading -> Tralling edge;
     * Clock Rate: fclk/4 */
	
  SET_MOSI_HI;
  SET_SCK_HI;
  SET_SS_HI;

  SREG = backupSREG;
}

// this function is used to gain exclusive access to the SPI bus
// and configure the correct settings.
void beginTrSPI(uint8_t spcr, uint8_t spsr)
{
  backupSPCR = SPCR;
  backupSPSR = SPSR;
  backupSREG = SREG;
    
  cli();
    
  SPCR = spcr;
  SPSR = spsr;
}

// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
uint8_t sendReadDataSPI(uint8_t data)
{
  SPDR = data;
  SPDR_TX_WAIT;
  
  return SPDR;
}

uint16_t sendReadWordDataSPI(uint16_t data)
{
  //by this one only r24 and r25 will be used (if -O2);
  SPDR_t in = {.val = data}, out;
  
  SPDR = in.msb;
  SPDR_TX_WAIT;
  out.msb = SPDR;
  
  SPDR = in.lsb;
  SPDR_TX_WAIT;
  out.lsb = SPDR;
  
  return out.val;
}

void sendData8_SPI1(uint8_t data)
{
  SPDR = data;
  SPDR_TX_WAIT;
}

void sendData16_SPI1(uint16_t data)
{
  //by this one only r24 and r25 will be used (if -O2);
  SPDR_t in = {.val = data};
  
  SPDR = in.msb;
  SPDR_TX_WAIT;
  
  SPDR = in.lsb;
  SPDR_TX_WAIT;
}

void sendReadArrSPI(void *buf, uint16_t count)
{
  uint8_t *p = (uint8_t *)buf;
  
  SPDR = *p;
  
  while (count--) {
    SPDR_TX_WAIT;
    *p++ = SPDR;
    SPDR = *(p + 1);
  }
  
  while (!(SPSR & (1<<SPIF))) ;
  *p = SPDR;
}

void sendArrSPI(uint8_t *buf, uint16_t size)
{
  uint8_t count;
  
  for(count = 0; count < size; count++){
    
    SPDR = buf[count];
    SPDR_TX_WAIT
  }
}

// After performing a group of transfers and releasing the chip select
// signal, this function allows others to access the SPI bus
void endTrSPI(void)
{
  SPCR = backupSPCR;
  SPSR = backupSPSR;
  SREG = backupSREG;
}
// --------------------------------------------------------- //

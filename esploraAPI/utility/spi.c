#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"


#define SC_SS_PIN  PB0   // system chip select for the atmega board
#define SCK_PIN    PB1
#define MOSI_PIN   PB2

#define SPI_PORTx  PORTB
#define SPI_DDRx   DDRB


#define SET_BIT(a,b)    (a |= (1<<b))
#define RESET_BIT(a,b)  (a &= ~ (1 << b))


#define SET_SCK_LOW     RESET_BIT(SPI_PORTx, SCK_PIN)
#define SET_MOSI_HI     SET_BIT(SPI_PORTx, MOSI_PIN)
#define SET_SCK_HI      SET_BIT(SPI_PORTx, SCK_PIN)
#define SET_SS_HI       SET_BIT(SPI_PORTx, SC_SS_PIN)

/*
 * The following NOP introduces a small delay that can prevent the wait
 * loop form iterating when running at the maximum speed. This gives
 * about 10% more speed, even if it seems counter-intuitive. At lower
 * speeds it is unnoticed.
 */
#define SPDR_TX_WAIT  asm volatile("nop"); while (!(SPSR & (1<<SPIF))) ;

typedef union {
    uint16_t val;
    struct {
        uint8_t lsb;
        uint8_t msb;
    };
} SPDR_t;    // this one will use only registers

void initSPI(void)
{
  uint8_t backupSREG = SREG;
  cli();
  
  SPI_DDRx |= ((1 << SC_SS_PIN) | (1 << SCK_PIN) | (1 << MOSI_PIN));
  SPI_PORTx |= ((1 << SC_SS_PIN) | (1 << SCK_PIN) | (1 << MOSI_PIN));

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.
  SPCR = (1<<SPE) | (1<<MSTR);
  SPSR = (1 << SPI2X);  // double speed
    /* Iterrupt: disable;
     * SPI: enable;
     * Data order: MSB;
     * Master/slave: Master;
     * Clock polarity: Rising;
     * Clock [hase: Leading -> Tralling edge;
     * Clock Rate: fclk/2 */
  
  SREG = backupSREG;
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

void sendArrSPI(uint8_t *buf, uint16_t size)
{
  uint8_t count;
  
  for(count = 0; count < size; count++) {
    SPDR = buf[count];
    SPDR_TX_WAIT
  }
}
// --------------------------------------------------------- //

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
  SPSR = (1<<SPI2X);  // double speed
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

// bData - 8bit data to be loaded in SPDR
// len - number of cycles to wait
inline void sendData8Dirt_SPI1(uint8_t bData, uint8_t len)
{
  // +2 mov cycles
  asm volatile(
    "out %[spdr], %[bData] \n"  //  1 - transmit byte
    "dec %[len]            \n"  //  1 - decrease count
    "brne .-4              \n"  //  1 -false 2-true; is it 0?

    :
    [bData] "=r" (bData),
    [len]   "=r" (len)

    :
    "[bData]" (bData),
    "[len]" (len),
    [spdr]  "I" (_SFR_IO_ADDR(SPDR))   // SPI data register

    : "cc"  // indicates what flags may be clobbered
  );
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

#include <avr/interrupt.h>

#include "tone.h"

#define PASTE(x,y)  x ## y
#define PORT(x)     PASTE(PORT,x)
//#define PIN(x)      PASTE(PIN,x)
#define DDR(x)      PASTE(DDR,x)

#define SOUND           D

#define SOUND_DDR       DDR(SOUND)
#define SOUND_PORT      PORT(SOUND)
#define SOUND_PORT_PIN  PD7

#define MAX_OCR_VAL     0xFFFF

#ifndef F_CPU
#define F_CPU 16000000L
#endif

volatile int32_t timer1_toggle_count;

void initSound(void)
{
  // 16 bit timer
  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3B |= ((1<<WGM32) | (1<<CS30));

  SOUND_DDR |= (1<<SOUND_PORT_PIN); // Enable output
}

// 1 param - frequency Hz
// 2 param - duration in milliseconds
void toneBuzz(uint16_t frequency, uint16_t duration)
{
  uint8_t prescalarbits = 0x1; // 0b001
  uint32_t ocr = F_CPU / frequency / 2 - 1;
  int32_t toggle_count = 2 * frequency * duration / 1000;

  if(ocr > MAX_OCR_VAL) {
    ocr = F_CPU / frequency / 2 / 64 - 1;
    prescalarbits = 0x3; // 0b011
  }
  
  TCCR3B = (TCCR3B & 0b11111000) | prescalarbits; // Set prescalar bits;

  //SOUND_PORT |= (1 << SOUND_PORT_PIN);            // Enable sound pin;

  OCR3A = ocr;                                    // Load value to OCR;
  timer1_toggle_count = toggle_count;             // Set toggle count;
  TIMSK3 |= (1<<OCIE3A);                          // Enable ISR.
}

ISR(TIMER3_COMPA_vect)
{
  if(timer1_toggle_count != 0) {
    SOUND_PORT ^= (1 << SOUND_PORT_PIN);
    
    if(timer1_toggle_count > 0)
      timer1_toggle_count--;

  } else {
    TIMSK3 &= ~(1<<OCIE3A);               // disable timer
    SOUND_PORT &= ~(1 << SOUND_PORT_PIN); // disable sound pin
  }
}

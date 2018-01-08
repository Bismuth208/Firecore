#include <avr/io.h>

#include "adc.h"

void setPrescallerADC(uint8_t ps)
{
  ADCSRA &= ~(1 << ADEN);
  
  switch(ps)
  {                                                   // equal sampling freq:
    case 0: { // 2
      // impossible to achive stable ADC
      ADCSRA = (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);  // 790'400 Hz
    } break;
    case 1: { // 4
      // impossible to achive stable ADC
      ADCSRA = (0<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);  // 395'200 Hz
    } break;
    case 2: { // 8
      ADCSRA = (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);  // 197'600 Hz
    } break;
    case 3: { // 16
      ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);  // 98'800 Hz
    } break;
    case 4: { // 32
      ADCSRA = (1<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);  // 38'400 Hz
    } break;
    case 5: { // 64
      ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);  // 19'200 Hz
    } break;
    case 6: { // 128
      // Arduino default and the only recommended value
      ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);  // 9'600 Hz
    } break;
    default: { // 64
      ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);
    } break;
  }

  ADCSRA |= (1 << ADEN);
}

void setChannelADC(uint8_t chADC)
{
  // Select ADC Channel ch must be 0-13
  ADMUX |= (chADC & ADC_CH_MASK);
#if 0
  if (chADC <= 5) {
  /* Note that ADC pins ADC7 and ADC6 do not have digital input buffers,
   * and therefore do not require Digital Input Disable bits.
  */
    DIDR0 |= (chADC & ADC_CH_MASK); // turn off the digital input for pin
  }
#endif
}

void initADC(void)
{
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  ADMUX |= (1<<REFS0); //Voltage reference from Avcc (5v)

  // ADC set to 64
  //setPrescallerADC(5);
  ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);  // 19'200 Hz
  ADCSRA |= (1 << ADEN);
  
  // AIN1, AIN0 Digital Input Disable
  DIDR1 |= (1<<AIN1D) | (1<<AIN0D);
}

uint16_t readADC(void)
{
  //Start Single conversion
  ADCSRA |= (1<<ADSC);

  // Wait for conversion to complete
  // ADSC is cleared when the conversion finished
  while(ADCSRA & (1<<ADSC));
   
  return ADC;
}

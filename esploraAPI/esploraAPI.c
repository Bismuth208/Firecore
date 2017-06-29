#include "esploraAPI.h"

// ------------------------------------ //
#define PASTE(x,y)  x ## y
#define PORT(x)     PASTE(PORT,x)
//#define PIN(x)      PASTE(PIN,x)
#define DDR(x)      PASTE(DDR,x)

// ------------------------------------ //
// defines for 74HC4067D muxtiplexer

#define MUXA_PIN      PF7
#define MUXB_PIN      PF6
#define MUXC_PIN      PF5
#define MUXD_PIN      PF4

#define MUX_READ_PIN  PF1

#define MUX_DDRX      DDRF
#define MUX_PORTX     PORTF

// ------------------------------------ //
// defines for RGB LED
#define LED_PIN_R     PC6
#define LED_PIN_B     PB5
#define LED_PIN_G     PB6

#define LED_PORTX_BG  PORTB
#define LED_PORTX_R   PORTC

#define LED_DDRX_R    DDRC
#define LED_DDRX_BG   DDRB

// ------------------------------------ //
// defines for Accelerometer
#define ACC_PIN_X      PF0
#define ACC_PIN_Y      PD6
#define ACC_PIN_Z      PD4

#define ACC_PORTX_X    PORTF
#define ACC_PORTX_YZ   PORTD

#define ACC_DDRX_X     DDRF
#define ACC_DDRX_YZ    DDRD

// ------------------------------------ //

#define SW_BTN_MIN_LVL 800
// ------------------------------------ //

void initMuxIO(void)
{
  // setup multiplexor IO control as output
  MUX_DDRX  |= ((1<<MUXD_PIN) | (1<<MUXC_PIN) | (1<<MUXB_PIN) | (1<<MUXA_PIN));
  
  // set analoge input from MUX as input and pull-up
  MUX_DDRX &= ~(1<<MUX_READ_PIN);
  MUX_PORTX |= (1<<MUX_READ_PIN);
}

void initLEDIO(void)
{
  // set as output
  LED_DDRX_BG |= (1<<LED_PIN_B) | (1<<LED_PIN_G);
  LED_DDRX_R  |= (1<<LED_PIN_R);
  // turn off
  LED_PORTX_R   &= ~(1<<LED_PIN_R);
  LED_PORTX_BG  &= ~((1<<LED_PIN_B) | (1<<LED_PIN_G));
}

void initAccelerometerIO(void)
{
  // set as input
  ACC_DDRX_X  &= ~(1<<ACC_PIN_X);
  ACC_DDRX_YZ &= ~((1<<ACC_PIN_Y) | (1<<ACC_PIN_Z));
  
  // pull-up
  ACC_PORTX_X |= (1<<ACC_PIN_X);
  ACC_DDRX_YZ |= ((1<<ACC_PIN_Y) | (1<<ACC_PIN_Z));
}

void initEsplora(void)
{
  // disable USB for 32u4
  USBCON = 0;
  
  // init Perephirial
  initSysTickTimer();
  initSPI();
  initSound();  // still not working... :/
  initADC();
  
  // init LCD
  initR(INITR_BLACKTAB);
  
  // setup IO
  initMuxIO();
  initLEDIO();
  initAccelerometerIO();
  
  // presetup as most used
  setChannelADC(ANALOG_MUX_CH);
}


// ------------------------------------ //
void setLEDValue(uint8_t pin, uint8_t state)
{
  volatile uint8_t *pPort;
  volatile uint8_t pPin;
  
  switch(pin)
  {
    case LED_R: {
      pPort = &LED_PORTX_R;
      pPin = LED_PIN_R;
    } break;
    
    case LED_G: {
      pPort = &LED_PORTX_BG;
      pPin = LED_PIN_G;
    } break;
      
    case LED_B: {
      pPort = &LED_PORTX_BG;
      pPin = LED_PIN_B;
    } break;
  }
  
  if(state) {
    *pPort |= (1<<pPin);
  } else {
    *pPort &= ~(1<<pPin);
  }
}

// ------------------------------------ //

uint16_t readAccelerometr(uint8_t axy)
{
  setChannelADC(axy);
  uint16_t val = readADC();
  
  setChannelADC(ANALOG_MUX_CH);
  return val;
}

// ---------- MUX --------- //
uint16_t getAnalogMux(uint8_t chMux)
{
  MUX_PORTX = ((MUX_PORTX & 0x0F) | ((chMux<<4)&0xF0));
  
  return readADC();
}

bool readSwitchButton(uint8_t btn)
{
  bool state = true;
  if(getAnalogMux(btn) > SW_BTN_MIN_LVL) { // state low == pressed
    state = false;
  }
  return state;
}

uint16_t readJoystic(uint8_t btn)
{
  return getAnalogMux(btn);
}

uint16_t readMic(void)
{
  return getAnalogMux(MIC_MUX_VAL);
}

uint16_t readLight(void)
{
  return getAnalogMux(LIGHT_MUX_VAL);
}

uint16_t readTemp(void)
{
  return getAnalogMux(TEMP_MUX_VAL);
}

uint16_t readLinear(void)
{
  return getAnalogMux(LINEAR_MUX_VAL);
}

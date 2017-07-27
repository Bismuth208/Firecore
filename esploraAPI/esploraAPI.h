/*
 * Author: Antonov Alexandr (Bismuth208)
 * e-mail: bismuth20883@gmail.com
 *
 *  THIS PROJECT IS PROVIDED FOR EDUCATION/HOBBY USE ONLY
 *  NO PROTION OF THIS WORK CAN BE USED IN COMMERIAL
 *  APPLICATION WITHOUT WRITTEN PERMISSION FROM THE AUTHOR
 *
 *  EVERYONE IS FREE TO POST/PUBLISH THIS ARTICLE IN
 *  PRINTED OR ELECTRONIC FORM IN FREE/PAID WEBSITES/MAGAZINES/BOOKS
 *  IF PROPER CREDIT TO ORIGINAL AUTHOR IS MENTIONED WITH LINKS TO
 *  ORIGINAL ARTICLE
 */

#ifndef _ESPLORA_API_H
#define _ESPLORA_API_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#include "utility/adc.h"
#include "utility/spi.h"
#include "utility/sfx.h"
#include "utility/systicktimer.h"
#include "utility/gfx.h"
#include "utility/st7735.h"
#include "utility/help.h"

// ------------------------------------ //
#define ACCEL_AXIS_X  ADC_CH0
#define ACCEL_AXIS_Y  ADC_CH9
#define ACCEL_AXIS_Z  ADC_CH8
// ------------------------------------ //

#define ANALOG_MUX_CH  ADC_CH1

// ------------------------------------ //
// values are bit mirrored for direct port set
#define SW_BTN_1_MUX   0
#define SW_BTN_2_MUX   8
#define SW_BTN_3_MUX   4
#define SW_BTN_4_MUX   12

#define LINEAR_MUX_VAL 2
#define LIGHT_MUX_VAL  10
#define TEMP_MUX_VAL   6
#define MIC_MUX_VAL    14

#define IN_A_MUX_VAL   1
#define IN_B_MUX_VAL   9

#define SW_J_MUX_VAL   5
#define X_J_MUX_VAL    13
#define Y_J_MUX_VAL    3
// ------------------------------------ //

#define LED_R  0
#define LED_G  1
#define LED_B  2

// ------------------------------------ //

#define BUTTON_A   SW_BTN_4_MUX
#define BUTTON_B   SW_BTN_1_MUX
#define BUTTON_X   SW_BTN_2_MUX
#define BUTTON_Y   SW_BTN_3_MUX
#define buttonIsPressed(a)  readSwitchButton(a)

// ------------------------------------ //

#define LINE_X     X_J_MUX_VAL
#define LINE_Y     Y_J_MUX_VAL
#define LR_OK      SW_J_MUX_VAL
#define getStickVal(a)  getJoyStickValue(a)

// ------------------------------------ //

#ifdef __cplusplus
extern "C"{
#endif
  
void initEsplora(void);

uint16_t readAccelerometr(uint8_t axy);

uint16_t getAnalogMux(uint8_t chMux);

bool readSwitchButton(uint8_t btn);
uint16_t readJoystic(uint8_t btn);
uint16_t readMic(void);
uint16_t readLight(void);
uint16_t readTemp(void);
uint16_t readLinear(void);
  
void setLEDValue(uint8_t pin, uint8_t state);
  
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_ESPLORA_API_H*/

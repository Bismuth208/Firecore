#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

void setPrescallerADC(uint8_t ps);
void setChannelADC(uint8_t chADC);
void initADC(void);
uint16_t readADC(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_ADC_H*/

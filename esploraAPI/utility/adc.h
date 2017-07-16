#ifndef _ADC_H
#define _ADC_H

#include <stdint.h>


#define ADC_CH_MASK    0x27
#define ADC_HI_CH_MASK 0x20

#define ADC_CH0    0x00
#define ADC_CH1    0x01
#define ADC_CH2    0x02
#define ADC_CH3    0x03
#define ADC_CH4    0x04
#define ADC_CH5    0x05
#define ADC_CH6    0x06
#define ADC_CH7    0x07
#define ADC_CH8    (ADC_HI_CH_MASK | ADC_CH0)
#define ADC_CH9    (ADC_HI_CH_MASK | ADC_CH1)
#define ADC_CH10   (ADC_HI_CH_MASK | ADC_CH2)
#define ADC_CH11   (ADC_HI_CH_MASK | ADC_CH3)
#define ADC_CH12   (ADC_HI_CH_MASK | ADC_CH4)
#define ADC_CH13   (ADC_HI_CH_MASK | ADC_CH5)

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

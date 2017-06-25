#ifndef _TONE_H
#define _TONE_H

#ifdef __cplusplus
extern "C"{
#endif

void initSound(void);
void toneBuzz(uint16_t frequency, unsigned long duration);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

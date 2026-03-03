#ifndef __AD_H
#define __AD_H
#include "stm32f10x.h"
extern uint16_t AD_Value[50]; 
//volatile uint8_t sampling_complete = 0; 
//volatile uint16_t sample_count = 0;
void TIM3_Init(void);
void sampling(void);
void AD_Init(void);
void All_Init(void);
#endif

#ifndef __PWM_H__
#define __PWM_H__

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);//用通道1
void PWM_SetCompare2(uint16_t Compare);//用通道2
void PWM_SetCompare3(uint16_t Compare);
#endif

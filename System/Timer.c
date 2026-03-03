#include "stm32f10x.h"                  // Device header
extern uint16_t Num;
void Timer_Init(void)
{
	/*¿ªÆôÊ±ÖÓ*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//¿ªÆôTIM2µÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);			//¿ªÆôPA
	
	/*ÅäÖÃA0¿Ú*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	/*ÅäÖÃÊ±ÖÓÔ´*/
	TIM_ETRConfig(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x0F);		//Ñ¡ÔñTIM2ÎªÍâ²¿Ê±ÖÓ£¬Èô²»µ÷ÓÃ´Ëº¯Êý£¬TIMÄ¬ÈÏÒ²ÎªÄÚ²¿Ê±ÖÓ
	
	/*Ê±»ùµ¥Ôª³õÊ¼»¯*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//¶¨Òå½á¹¹Ìå±äÁ¿
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//Ê±ÖÓ·ÖÆµ£¬Ñ¡Ôñ²»·ÖÆµ£¬´Ë²ÎÊýÓÃÓÚÅäÖÃÂË²¨Æ÷Ê±ÖÓ£¬²»Ó°ÏìÊ±»ùµ¥Ôª¹¦ÄÜ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//¼ÆÊýÆ÷Ä£Ê½£¬Ñ¡ÔñÏòÉÏ¼ÆÊý
	TIM_TimeBaseInitStructure.TIM_Period = 800 - 1;				//¼ÆÊýÖÜÆÚ£¬¼´ARRµÄÖµ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				//Ô¤·ÖÆµÆ÷£¬¼´PSCµÄÖµ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//ÖØ¸´¼ÆÊýÆ÷£¬¸ß¼¶¶¨Ê±Æ÷²Å»áÓÃµ½
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//½«½á¹¹Ìå±äÁ¿½»¸øTIM_TimeBaseInit£¬ÅäÖÃTIM2µÄÊ±»ùµ¥Ôª	
	
	/*ÖÐ¶ÏÊä³öÅäÖÃ*/
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);						//Çå³ý¶¨Ê±Æ÷¸üÐÂ±êÖ¾Î»
																//TIM_TimeBaseInitº¯ÊýÄ©Î²£¬ÊÖ¶¯²úÉúÁË¸üÐÂÊÂ¼þ
																//Èô²»Çå³ý´Ë±êÖ¾Î»£¬Ôò¿ªÆôÖÐ¶Ïºó£¬»áÁ¢¿Ì½øÈëÒ»´ÎÖÐ¶Ï
																//Èç¹û²»½éÒâ´ËÎÊÌâ£¬Ôò²»Çå³ý´Ë±êÖ¾Î»Ò²¿É
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					//¿ªÆôTIM2µÄ¸üÐÂÖÐ¶Ï
	
	/*NVICÖÐ¶Ï·Ö×é*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//ÅäÖÃNVICÎª·Ö×é2
																//¼´ÇÀÕ¼ÓÅÏÈ¼¶·¶Î§£º0~3£¬ÏìÓ¦ÓÅÏÈ¼¶·¶Î§£º0~3
																//´Ë·Ö×éÅäÖÃÔÚÕû¸ö¹¤³ÌÖÐ½öÐèµ÷ÓÃÒ»´Î
																//ÈôÓÐ¶à¸öÖÐ¶Ï£¬¿ÉÒÔ°Ñ´Ë´úÂë·ÅÔÚmainº¯ÊýÄÚ£¬whileÑ­»·Ö®Ç°
																//Èôµ÷ÓÃ¶à´ÎÅäÖÃ·Ö×éµÄ´úÂë£¬ÔòºóÖ´ÐÐµÄÅäÖÃ»á¸²¸ÇÏÈÖ´ÐÐµÄÅäÖÃ
	
	/*NVICÅäÖÃ*/
	NVIC_InitTypeDef NVIC_InitStructure;						//¶¨Òå½á¹¹Ìå±äÁ¿
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//Ñ¡ÔñÅäÖÃNVICµÄTIM2Ïß
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//Ö¸¶¨NVICÏßÂ·Ê¹ÄÜ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//Ö¸¶¨NVICÏßÂ·µÄÇÀÕ¼ÓÅÏÈ¼¶Îª2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//Ö¸¶¨NVICÏßÂ·µÄÏìÓ¦ÓÅÏÈ¼¶Îª1
	NVIC_Init(&NVIC_InitStructure);								//½«½á¹¹Ìå±äÁ¿½»¸øNVIC_Init£¬ÅäÖÃNVICÍâÉè
	
	/*TIMÊ¹ÄÜ*/
	TIM_Cmd(TIM2, ENABLE);			//Ê¹ÄÜTIM2£¬¶¨Ê±Æ÷¿ªÊ¼ÔËÐÐ
}

uint16_t  TIM_GET(void)
{
	return TIM_GetCounter(TIM2);
}



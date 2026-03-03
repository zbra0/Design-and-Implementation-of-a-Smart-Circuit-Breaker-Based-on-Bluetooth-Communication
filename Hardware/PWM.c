#include "stm32f10x.h"                  // Device header


//步骤：RCC开启时钟 TIM、GPIO外设的时钟
 //     配置时基单元 包括时钟源的选择
 //     配置输出比较单元
void PWM_Init(void)//PWM初始化
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//TIM2时钟APB1总线的外设 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//外设时钟GPIOA使能
	

	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP ;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;;//选中2号口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO初始化函数  地址传递 低电平 可点亮灯
	
	
	TIM_InternalClockConfig(TIM2);//想选择为内部时钟
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;//定义结构体 再引出结构体成员
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//指定时钟分频  这里选择不分频 即1分频
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; //计数模式  选择向上计数
	//以下三个为时基单元
	TIM_TimeBaseInitStruct.TIM_Period=100-1;//ARR自动重装器的值   实现1s记一次
	TIM_TimeBaseInitStruct.TIM_Prescaler=36-1;//PSC预分频器的值  T=F/（p+1）/（q+1）推出为1s

  TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;//重复计数器的值 这里并不需要
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//此函数为结构体赋初值  再更改需要的值
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//设置输出模式
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//设置输出比较的极性  高极性
	TIM_OCInitStructure.TIM_OutputState= TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse=0;//用来设置CCR  CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	
	TIM_Cmd(TIM2, ENABLE);
}

		void PWM_SetCompare1(uint16_t Compare)
		{
			TIM_SetCompare1(TIM2, Compare);
		}

		void PWM_SetCompare2(uint16_t Compare)
		{
			TIM_SetCompare2(TIM2, Compare);
		}

		void PWM_SetCompare3(uint16_t Compare)
		{
			TIM_SetCompare3(TIM2, Compare);
		}

//void PWM_SetCompare4(uint16_t Compare)
//{
//	TIM_SetCompare4(TIM2, Compare);
//}




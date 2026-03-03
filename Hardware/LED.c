#include "stm32f10x.h"                  // Device header

void LED_Init(void)//LED初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//外设时钟GPIOA使能
	
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_6;//选中1/6号口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO初始化函数  地址传递 低电平 可点亮灯
	GPIO_SetBits(GPIOA,GPIO_Pin_1 |GPIO_Pin_6);//将1、6脚置高电平
	

}

 
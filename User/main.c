#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "AD.h"
#include "math.h"
#include "Timer.h"
#define Electric_Resist 0.5   //0.3
uint16_t Data1;
double current=0,current1=0;
float num=1;
float sum=0;
float *youxiao;
float youxiao_value=0; 
double crms = 0;
float Ir=4.0;
float *Ir_p;
float Q=0;
float Qm=500;
int main(void)
{ 

	Serial_Init();
	LED_Init();
	youxiao=&youxiao_value;
	Ir_p=&Ir;
	All_Init();
    // 启动采样
    TIM_Cmd(TIM3, ENABLE);      // 启动TIM3，开始触发ADC采样
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	while (1)
	{	
		sampling();
		for(int i=0;i<50;i++)
		{
			current=(double)AD_Value[i];
			current1=current*3.3/4096/Electric_Resist;
			sum=sum+current1*current1;	
		}
		*youxiao=sqrt(sum/50);
        crms = youxiao_value;		
		sum=0;
		Serial_Printf("电流: %.2fA  当前阈值为: %.2fA\r\n",crms,Ir);
		if (crms < 0.9*Ir )
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			if (Q > 20)
			{
				Q = Q - 20;
			}
			else 
				Q=0;
			 
		}
		else if (crms < 1.15*Ir)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_8);
			 if (Q > 10)
			{
				Q = Q - 10;
			}
			else 
				Q=0;
				
		}
		else if (crms < 1.8*Ir)//5.2
		{
			Q = Q + 5;
			if (Q >= Qm)
			{
				 GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			}
		}
		else 
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);//white
		}
		
					
	}
}

void USART1_IRQHandler(void)//蓝牙模块
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Data1=USART_ReceiveData(USART1);
		if(Data1==0x30)GPIO_SetBits(GPIOA,GPIO_Pin_1);//合闸
		if(Data1==0x31)GPIO_ResetBits(GPIOA,GPIO_Pin_1);//分闸
		if(Data1==0x32)(*Ir_p)+=0.1;
		if(Data1==0x33 && *Ir_p>=0.1)(*Ir_p)-=0.1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}



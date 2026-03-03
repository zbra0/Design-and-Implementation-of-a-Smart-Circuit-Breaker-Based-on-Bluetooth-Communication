#include "stm32f10x.h"  // 包含STM32F10x系列微控制器的标准外设库头文件

uint16_t AD_Value[50];  // 定义一个大小为50的数组，用于存储ADC转换得到的数值
volatile uint8_t sampling_complete = 0;  // 定义一个易变的标志位，用于指示50次采样是否全部完成
volatile uint16_t sample_count = 0;      // 定义一个易变的计数器，用于记录当前已完成的采样次数

/**
  * 函    数：TIM3初始化，配置为以5kHz频率触发ADC采样（即每0.2ms触发一次）
  * 参    数：无
  * 返 回 值：无
  */
void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; // 定义一个定时器时基初始化结构体
    
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 使能APB1总线上的TIM3外设时钟
    
    /*时基单元初始化*/
    TIM_TimeBaseInitStructure.TIM_Period = 288 - 1;        // 设置自动重装载寄存器的值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 100 - 1;     // 设置预分频器的值
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分频因子，与死区时间相关，此处不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 设置计数器为向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);    // 将配置写入TIM3寄存器
    
    /*使能TIM3更新中断*/
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
    
    /*设置中断优先级*/
    NVIC_InitTypeDef NVIC_InitStructure; // 定义一个嵌套向量中断控制器(NVIC)初始化结构体
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // 指定要配置的中断通道为TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 设置抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 设置子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能该中断通道
    NVIC_Init(&NVIC_InitStructure); // 将配置写入NVIC寄存器，完成中断优先级设置
}

/**
  * 函    数：ADC和DMA初始化
  * 参    数：无
  * 返 回 值：无
  */
void AD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体
    ADC_InitTypeDef ADC_InitStructure;   // 定义ADC初始化结构体
    DMA_InitTypeDef DMA_InitStructure;   // 定义DMA初始化结构体
    
    /*开启时钟*/
    // 使能APB2总线上的ADC1和GPIOA外设时钟，以及AHB总线上的DMA1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    /*设置ADC时钟*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 配置ADC时钟为APB2时钟(PCLK2)的6分频。PCLK2通常为72MHz，故ADC时钟为12MHz。
    
    /*GPIO初始化 - PA0为模拟输入*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // 选择要初始化的引脚为PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 将配置应用到GPIOA端口
    
    /*DMA初始化*/
    DMA_DeInit(DMA1_Channel1); // 将DMA1通道1寄存器复位为默认值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 50; // 设置DMA传输数据量的大小为50（即数组长度）
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 设置传输后外设地址不递增（因为始终从同一个ADC数据寄存器读数）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 设置传输后内存地址递增，以便将数据顺序存入数组
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 设置外设数据宽度为半字（16位，与ADC数据寄存器宽度匹配）
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 设置内存数据宽度为半字（16位，与数组元素类型匹配）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 设置为正常模式：传输完设定的数据量后，DMA通道自动停止
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置DMA通道优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 禁用内存到内存的传输模式（此处是外设到内存）
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // 将配置写入DMA1通道1寄存器
    
    /*ADC初始化*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 设置ADC为独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁用扫描模式（因为是单通道）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 禁用连续转换模式，即单次转换模式，每次都需要触发
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 不使用外部触发，将使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 设置ADC数据右对齐（最低位为有效位）
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 指定规则组序列中要转换的通道数为1
    ADC_Init(ADC1, &ADC_InitStructure); // 将配置写入ADC1寄存器
    
    /*ADC通道配置*/
    // 将ADC1的通道0（对应GPIO PA0）配置为规则组序列中的第1个转换通道，采样时间为239.5个ADC时钟周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
    
    /*使能ADC*/
    ADC_Cmd(ADC1, ENABLE); // 使能ADC1外设
    
    /*ADC校准*/
    ADC_ResetCalibration(ADC1); // 启动ADC1的复位校准过程
    while(ADC_GetResetCalibrationStatus(ADC1)); // 等待复位校准完成
    ADC_StartCalibration(ADC1); // 启动ADC1的校准过程
    while(ADC_GetCalibrationStatus(ADC1)); // 等待校准完成
}

/**
  * 函    数：TIM3中断服务函数
  * 参    数：无
  * 返 回 值：无
  */
void TIM3_IRQHandler(void)
{
    // 检查是否发生了TIM3的“更新”中断
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIM3的“更新”中断标志位，防止重复进入中断
        
        /*启动ADC转换*/
        ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 通过软件命令启动一次ADC1的转换
        
        sample_count++; // 采样次数计数器加1
        
        /*采样达到50次后停止定时器*/
        if (sample_count >= 50)
        {
            TIM_Cmd(TIM3, DISABLE); // 禁用TIM3定时器，停止产生中断
            sampling_complete = 1; // 将采样完成标志位置1，通知主程序
        }
    }
}

/**
  * 函    数：采样函数 - 调用一次以启动一个完整的50点采样过程
  * 参    数：无
  * 返 回 值：无
  */
void sampling(void)
{
    /*复位采样状态*/
    sampling_complete = 0; // 清除采样完成标志
    sample_count = 0;      // 清除采样计数器
    
    /*使能DMA传输*/
    DMA_Cmd(DMA1_Channel1, ENABLE); // 使能DMA1通道1，准备进行数据传输
    
    /*使能ADC的DMA*/
    ADC_DMACmd(ADC1, ENABLE); // 使能ADC1的DMA请求，ADC转换完成后会自动触发DMA搬运
    
    /*设置DMA传输数据量*/
    DMA_SetCurrDataCounter(DMA1_Channel1, 50); // 设置DMA1通道1本次要传输的数据量为50
    
    /*启动定时器开始采样*/
    TIM_Cmd(TIM3, ENABLE); // 使能TIM3定时器，开始定时触发ADC转换
    
    /*清除所有DMA标志位*/
    DMA_ClearFlag(DMA1_FLAG_TC1 | DMA1_FLAG_HT1 | DMA1_FLAG_TE1); // 清除DMA1通道1的全部标志位
    
    /*重新使能DMA*/
    DMA_Cmd(DMA1_Channel1, ENABLE); // 重新使能DMA1通道1
    
    /*等待采样完成*/
    while(sampling_complete == 0); // 循环等待，直到TIM3中断中将`sampling_complete`标志置1
    

}

/**
  * 函    数：初始化所有外设
  * 参    数：无
  * 返 回 值：无
  */
void All_Init(void)
{
    TIM3_Init(); // 初始化定时器TIM3
    AD_Init();   // 初始化ADC和DMA
}
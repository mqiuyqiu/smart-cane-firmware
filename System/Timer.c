/**
  ******************************************************************************
  * @file    Timer.c
  * @brief   定时器配置：Timer2用于超声波Echo脉宽计时
  * @details Timer2时钟72MHz不分频，自动重装载7200-1 -> 10ms中断一次。
  *          Echo(PB5)为高电平时，每次中断Time加1，即Time单位为10ms。
  *          配合HC-SR04使用：距离(cm) = Time * 1.7。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header

extern uint16_t Time;    // 超声波Echo高电平计时变量（在HC_SR04.c中定义）
uint32_t sys_tick = 0;

/**
  * @brief  初始化Timer2：10ms中断，用于超声波Echo脉宽测量
  * @param  无
  * @retval 无
  */
void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_InternalClockConfig(TIM2);    // 使用内部时钟72MHz

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 7200 - 1;      // 72MHz/7200 = 10kHz -> 10ms溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;       // 不分频
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Timer2中断服务函数：Echo为高电平时累加Time（10ms一次）
  */
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		/* Echo(PB5)为高电平说明超声波正在回波计时 */
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 1)
		{
			Time++;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

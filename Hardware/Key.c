#include "stm32f10x.h"                  // Device header
#include "Delay.h"

volatile uint8_t Key_Flag = 0;
volatile uint16_t Key_Press_Time = 0;   // 按键按下时长
volatile uint8_t Key_Down_Flag = 0;     // 按键物理按下标志（消抖后）

void SOSKey_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, DISABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t Key_GetState(void) {
	if (Key_Flag) {
		Key_Flag = 0;
		return 1;
	}
	return 0;
}

void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		uint8_t pin_level = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1); // 读取当前引脚电平
		uint8_t cnt = 0;
		while (cnt < 20)
		{
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == pin_level)
			{
				cnt++;
			}
			else
			{
				cnt = 0;
			}
		}
		if (pin_level == 0)
		{
			Key_Down_Flag = 1;    // 标记按键按下
			Key_Press_Time = 0;   // 重置长按计时
			TIM_SetCounter(TIM4, 0); // 重置TIM4计数器
			TIM_Cmd(TIM4, ENABLE);  // 开启TIM4，开始计时
		}
		else // 按键松开
		{
			Key_Down_Flag = 0;    // 标记按键松开
			TIM_Cmd(TIM4, DISABLE); // 关闭TIM4，停止计时
			Key_Press_Time = 0;   // 重置计时
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if (Key_Down_Flag) // 仅按键按下时计时
		{
			Key_Press_Time++; // 10ms累加一次
			if (Key_Press_Time >= 200) // 200*10ms=2000ms=2秒
			{
				Key_Flag = 1;         // 置SOS触发标志
				Key_Down_Flag = 0;    // 停止计时
				TIM_Cmd(TIM4, DISABLE); // 关闭TIM4
				Key_Press_Time = 0;   // 重置计时
			}
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除中断标志
	}
}

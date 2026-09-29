/**
  ******************************************************************************
  * @file    Key.c
  * @brief   SOS求救按键驱动
  * @details 按键接PB1，内部上拉，按下接地（低电平）。
  *          采用EXTI双边沿中断检测按下/松开，TIM4计时判断长按：
  *          - 按下后启动TIM4（10ms中断），累计200次=2秒
  *          - 达到2秒后置Key_Flag=1，主循环读取后触发SOS报警
  *          - 松开则停止计时并清零
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

volatile uint8_t  Key_Flag = 0;        // SOS触发标志：主循环读取后自动清零
volatile uint16_t Key_Press_Time = 0;  // 按键按下时长（单位：10ms）
volatile uint8_t  Key_Down_Flag = 0;   // 按键物理按下标志（消抖后）

/**
  * @brief  初始化SOS按键：PB1上拉输入、EXTI双边沿中断、TIM4定时计时
  * @param  无
  * @retval 无
  */
void SOSKey_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* PB1配置为上拉输入 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 配置PB1为EXTI1外部中断线 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  // 上升沿(松开)+下降沿(按下)都触发
	EXTI_Init(&EXTI_InitStructure);

	/* TIM4：预分频720-1 -> 72MHz/720=100kHz，自动重装载1000-1 -> 10ms中断一次 */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, DISABLE);    // 默认关闭，按键按下时才启动

	/* NVIC优先级组1：EXTI1最高(0,0)，TIM4次之(1,0) */
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

/**
  * @brief  主循环调用此函数查询是否触发SOS
  * @retval 1=SOS已触发（读取后自动清零），0=未触发
  */
uint8_t Key_GetState(void) {
	if (Key_Flag) {
		Key_Flag = 0;
		return 1;
	}
	return 0;
}

/**
  * @brief  EXTI1中断服务函数：检测按键按下/松开，软件消抖后启动/停止计时
  */
void EXTI1_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		uint8_t pin_level = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
		uint8_t cnt = 0;
		/* 软件消抖：连续20次（约20ms）电平稳定才认为有效 */
		while (cnt < 20)
		{
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == pin_level)
				cnt++;
			else
				cnt = 0;
		}

		if (pin_level == 0)          // 按键按下（低电平）
		{
			Key_Down_Flag = 1;       // 标记按下
			Key_Press_Time = 0;      // 重置长按计时
			TIM_SetCounter(TIM4, 0);  // 计数器清零
			TIM_Cmd(TIM4, ENABLE);    // 启动TIM4开始计时
		}
		else                          // 按键松开（高电平）
		{
			Key_Down_Flag = 0;       // 标记松开
			TIM_Cmd(TIM4, DISABLE);   // 停止计时
			Key_Press_Time = 0;      // 清零（未达到长按阈值）
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

/**
  * @brief  TIM4中断服务函数：每10ms进入一次，累计按键长按时间
  * @note   累计200次=2秒后置Key_Flag=1，触发SOS报警
  */
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		if (Key_Down_Flag)            // 仅按键保持按下时计时
		{
			Key_Press_Time++;         // 10ms累加一次
			if (Key_Press_Time >= 200) // 200*10ms=2000ms=2秒
			{
				Key_Flag = 1;          // 置SOS触发标志
				Key_Down_Flag = 0;     // 停止计时
				TIM_Cmd(TIM4, DISABLE);
				Key_Press_Time = 0;
			}
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

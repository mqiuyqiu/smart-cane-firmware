/**
  ******************************************************************************
  * @file    LED.c
  * @brief   双路LED警示灯驱动
  * @details LED1接PA6，LED2接PA7，推挽输出，低电平点亮（共阳接法）。
  *          用于前方障碍预警闪烁、SOS报警交替闪烁等声光提示。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header

/**
  * @brief  初始化LED引脚：PA6、PA7推挽输出，默认熄灭（高电平）
  * @param  无
  * @retval 无
  */
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7);   // 默认高电平=灯灭
}

/**
  * @brief  点亮LED1（PA6输出低电平）
  */
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}

/**
  * @brief  熄灭LED1（PA6输出高电平）
  */
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}

/**
  * @brief  翻转LED1状态（亮变灭、灭变亮）
  */
void LED1_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_6) == 0)
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}

/**
  * @brief  点亮LED2（PA7输出低电平）
  */
void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}

/**
  * @brief  熄灭LED2（PA7输出高电平）
  */
void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
}

/**
  * @brief  翻转LED2状态
  */
void LED2_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7) == 0)
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
}

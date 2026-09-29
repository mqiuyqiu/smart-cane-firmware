/**
  ******************************************************************************
  * @file    Buzzer.c
  * @brief   蜂鸣器报警驱动
  * @details 蜂鸣器接PB12，推挽输出，低电平鸣响。
  *          用于跌倒检测报警等场景的声音提示。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header

/**
  * @brief  初始化蜂鸣器引脚：PB12推挽输出，默认关闭（高电平）
  * @param  无
  * @retval 无
  */
void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);    // 默认高电平=蜂鸣器关闭
}

/**
  * @brief  蜂鸣器鸣响（PB12输出低电平）
  */
void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

/**
  * @brief  蜂鸣器停止（PB12输出高电平）
  */
void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/**
  * @brief  翻转蜂鸣器状态
  */
void Buzzer_Turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 0)
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

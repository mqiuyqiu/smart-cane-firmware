/**
  ******************************************************************************
  * @file    HC_SR04.c
  * @brief   HC-SR04超声波测距模块驱动
  * @details Trig接PB0（输出），Echo接PB5（输入，同时接入Timer2计数脉宽）。
  *          工作原理：Trig发>10us高电平触发，模块发出8个40kHz超声脉冲，
  *          Echo输出与距离成正比的高电平脉宽。Timer2每10us中断一次累加Time，
  *          距离(cm) ≈ Time * 0.017 * 100 ≈ Time * 1.7（声速340m/s，往返除2）。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header
#include "HC_SR04.h"
#include "Timer.h"
#include "Delay.h"

uint16_t Time;    // Echo高电平持续时间计数（单位：10us，由Timer2中断累加）

/**
  * @brief  初始化超声波引脚：Trig推挽输出，Echo浮空输入
  * @param  无
  * @retval 无
  */
void HCSR04_Init(void)
{
	RCC_APB2PeriphClockCmd(Trig_RCC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;

	/* Trig引脚：推挽输出 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = Trig_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Trig_Port, &GPIO_InitStruct);

	/* Echo引脚：浮空输入 */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = Echo_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Echo_Port, &GPIO_InitStruct);

	GPIO_ResetBits(Trig_Port, Trig_Pin);    // Trig默认低电平
}

/**
  * @brief  发送触发信号：Trig拉高25us后拉低，启动一次测距
  * @param  无
  * @retval 无
  */
void HCSR04_Start(void)
{
	Time = 0;                               // 清空上次计时
	GPIO_SetBits(Trig_Port, Trig_Pin);
	Delay_us(25);                           // 触发脉冲宽度>10us即可
	GPIO_ResetBits(Trig_Port, Trig_Pin);
}

/**
  * @brief  获取一次测距结果
  * @param  无
  * @retval 距离值，单位cm（范围约2~400cm）
  */
uint16_t HCSR04_GetValue()
{
	HCSR04_Start();
	Delay_ms(70);                           // 等待超声回波（70ms内完成一次测量）
	return Time * 1.7;  // 简化公式：Time(10us) * 0.017cm/us = Time * 1.7cm
}

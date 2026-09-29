#include "stm32f10x.h"                  // Device header
#include "HC_SR04.h"
#include "Timer.h"
#include "Delay.h"

uint16_t Time;

void HCSR04_Init(void)
{
	RCC_APB2PeriphClockCmd(Trig_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = Trig_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Trig_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = Echo_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Echo_Port, &GPIO_InitStruct);
	
	GPIO_ResetBits(Trig_Port, Trig_Pin);
	
}

void HCSR04_Start(void)
{
	Time = 0;
	GPIO_SetBits(Trig_Port, Trig_Pin);
	Delay_us(25);
	GPIO_ResetBits(Trig_Port, Trig_Pin);
}

uint16_t HCSR04_GetValue()
{
	HCSR04_Start();
	Delay_ms(70);
	return Time * 1.7;  // 简化公式（等价于(Time*0.0001)*34000/2）
}

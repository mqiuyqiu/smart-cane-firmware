#include "JQ8900.h"
#include "Delay.h" 

// 1. IO1独立初始化：推挽输出，默认拉高（不触发）
void JQ8900_IO1_Init(void)
{
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(JQ8900_IO1_RCC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出（驱动JQ8900 IO触发）
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // 50MHz速率
    GPIO_Init(JQ8900_IO1_PORT, &GPIO_InitStruct);
    
    GPIO_SetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN);  // 默认拉高（不触发）
}

// 2. IO2独立初始化：推挽输出，默认拉高
void JQ8900_IO2_Init(void)
{
    RCC_APB2PeriphClockCmd(JQ8900_IO2_RCC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO2_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JQ8900_IO2_PORT, &GPIO_InitStruct);
    
    GPIO_SetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);  // 默认拉高
}

// 3. IO3独立初始化：推挽输出，默认拉高
void JQ8900_IO3_Init(void)
{
    RCC_APB2PeriphClockCmd(JQ8900_IO3_RCC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO3_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JQ8900_IO3_PORT, &GPIO_InitStruct);
    
    GPIO_SetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);  // 默认拉高
}

// 4. JQ8900总初始化：调用3个独立引脚初始化函数
void JQ8900_Init(void)
{
    JQ8900_IO1_Init();
    JQ8900_IO2_Init();
    JQ8900_IO3_Init();
}

// 5. IO1触发播放（拉低20ms，JQ8900识别触发）
void JQ8900_Play_IO1(void)
{
    GPIO_ResetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN); 
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN);
}

// 6. IO2触发播放
void JQ8900_Play_IO2(void)
{
    GPIO_ResetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);
}

// 7. IO3触发播放
void JQ8900_Play_IO3(void)
{
    GPIO_ResetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);
}

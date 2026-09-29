/**
  ******************************************************************************
  * @file    JQ8900.c
  * @brief   JQ8900语音播报模块驱动
  * @details JQ8900通过IO口触发播放：引脚拉低100ms后松开，即触发对应语音。
  *          3路触发引脚：
  *            IO1(PB13) -> 播放"前方障碍物"提示音
  *            IO2(PB14) -> 播放"SOS求救"语音
  *            IO3(PB15) -> 播放"跌倒报警"语音
  ******************************************************************************
  */

#include "JQ8900.h"
#include "Delay.h"

/**
  * @brief  初始化IO1(PB13)：推挽输出，默认拉高（不触发）
  */
void JQ8900_IO1_Init(void)
{
    RCC_APB2PeriphClockCmd(JQ8900_IO1_RCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO1_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JQ8900_IO1_PORT, &GPIO_InitStruct);

    GPIO_SetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN);  // 默认高电平=不触发
}

/**
  * @brief  初始化IO2(PB14)：推挽输出，默认拉高
  */
void JQ8900_IO2_Init(void)
{
    RCC_APB2PeriphClockCmd(JQ8900_IO2_RCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO2_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JQ8900_IO2_PORT, &GPIO_InitStruct);

    GPIO_SetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);
}

/**
  * @brief  初始化IO3(PB15)：推挽输出，默认拉高
  */
void JQ8900_IO3_Init(void)
{
    RCC_APB2PeriphClockCmd(JQ8900_IO3_RCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = JQ8900_IO3_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(JQ8900_IO3_PORT, &GPIO_InitStruct);

    GPIO_SetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);
}

/**
  * @brief  JQ8900总初始化：依次初始化3路触发IO
  */
void JQ8900_Init(void)
{
    JQ8900_IO1_Init();
    JQ8900_IO2_Init();
    JQ8900_IO3_Init();
}

/**
  * @brief  触发IO1播放：拉低100ms后松开（JQ8900检测下降沿触发）
  */
void JQ8900_Play_IO1(void)
{
    GPIO_ResetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN);
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO1_PORT, JQ8900_IO1_PIN);
}

/**
  * @brief  触发IO2播放
  */
void JQ8900_Play_IO2(void)
{
    GPIO_ResetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO2_PORT, JQ8900_IO2_PIN);
}

/**
  * @brief  触发IO3播放
  */
void JQ8900_Play_IO3(void)
{
    GPIO_ResetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);
    Delay_ms(100);
    GPIO_SetBits(JQ8900_IO3_PORT, JQ8900_IO3_PIN);
}

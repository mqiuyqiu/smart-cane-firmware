#ifndef __JQ8900_H
#define __JQ8900_H

#include "stm32f10x.h"

// 1. 定义JQ8900的3个触发引脚
#define JQ8900_IO1_PORT GPIOB
#define JQ8900_IO1_PIN  GPIO_Pin_13
#define JQ8900_IO1_RCC  RCC_APB2Periph_GPIOB

#define JQ8900_IO2_PORT GPIOB
#define JQ8900_IO2_PIN  GPIO_Pin_14
#define JQ8900_IO2_RCC  RCC_APB2Periph_GPIOB

#define JQ8900_IO3_PORT GPIOB
#define JQ8900_IO3_PIN  GPIO_Pin_15
#define JQ8900_IO3_RCC  RCC_APB2Periph_GPIOB

// 2. 声明独立初始化函数（每个引脚单独初始化）
void JQ8900_IO1_Init(void);  // PB12初始化（对应JQ8900 IO1）
void JQ8900_IO2_Init(void);  // PB13初始化（对应JQ8900 IO2）
void JQ8900_IO3_Init(void);  // PB14初始化（对应JQ8900 IO3）
void JQ8900_Init(void);      // 总初始化（调用上述3个独立函数）

// 3. 声明独立触发函数（每个引脚单独触发播放）
void JQ8900_Play_IO1(void);
void JQ8900_Play_IO2(void);
void JQ8900_Play_IO3(void);

#endif

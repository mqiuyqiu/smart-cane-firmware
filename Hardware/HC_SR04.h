#ifndef __HCSR04_H
#define __HCSR04_H

#define Trig_Port GPIOB
#define Trig_Pin GPIO_Pin_0
#define Trig_RCC RCC_APB2Periph_GPIOB

#define Echo_Port GPIOB
#define Echo_Pin GPIO_Pin_5
#define Echo_RCC RCC_APB2Periph_GPIOB

void HCSR04_Init(void);
uint16_t HCSR04_GetValue(void);

#endif 

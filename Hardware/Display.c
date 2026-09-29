#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Buzzer.h"
#include "Delay.h"
#include "HC_SR04.h"
#include "MPU6050.h"
#include <stddef.h>  // 添加NULL定义的头文件

extern const uint8_t BMP_Warning[];

uint16_t distance_cm = 0;       // 距离（初始化0）
int16_t AccX = 0, AccY = 0, AccZ = 0;

void Refresh_Display(void)
{
	MPU6050_GetData(&AccX, &AccY, &AccZ, NULL, NULL, NULL);
	OLED_ShowSignedNum(2, 3, AccX, 5);   
    OLED_ShowSignedNum(3, 3, AccY, 5);   
    OLED_ShowSignedNum(4, 3, AccZ, 5);
	distance_cm = HCSR04_GetValue();
	OLED_ShowNum(1, 6, distance_cm, 3);
}
void Display(void)
{
	OLED_Clear();
	// X轴加速度
	OLED_ShowString(2, 1, "X:");
    OLED_ShowSignedNum(2, 3, AccX, 5);

    // Y轴加速度
    OLED_ShowString(3, 1, "Y:");
    OLED_ShowSignedNum(3, 3, AccY, 5);
    
    // Z轴加速度
    OLED_ShowString(4, 1, "Z:");
    OLED_ShowSignedNum(4, 3, AccZ, 5);
	
	OLED_ShowCN(1, 1, 11, 1);
	OLED_ShowCN(1, 2, 12, 1);
	OLED_ShowChar(1, 5, ':');
	OLED_ShowNum(1, 6, distance_cm, 3);
	OLED_ShowString(1, 9, "cm"); 
}

void SOS(void)
{
	for (int i = 0; i < 5; i++){
	OLED_Clear();
	OLED_ShowString(2, 5, "SOS");
	OLED_ShowCN(2, 5, 3, 1);
	OLED_ShowCN(2, 6, 2, 1);
	LED1_ON();
	LED2_OFF();
	Buzzer_ON();
	Delay_ms(500);
	OLED_Clear();
	OLED_ShowBMP(32, 0, 64, 64, BMP_Warning,1);
	LED1_OFF();
	LED2_ON();
	Buzzer_OFF();
	Delay_ms(500);
	LED2_OFF();
	OLED_Clear();
	}
}

/**
  ******************************************************************************
  * @file    Display.c
  * @brief   OLED显示刷新与SOS声光报警逻辑
  * @details Refresh_Display()：实时刷新距离和三轴加速度数值
  *          Display()：绘制完整界面布局（标题+数值）
  *          SOS()：SOS触发时LED交替闪烁+蜂鸣器+OLED告警图，循环5次
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Buzzer.h"
#include "Delay.h"
#include "HC_SR04.h"
#include "MPU6050.h"
#include <stddef.h>  // NULL定义

extern const uint8_t BMP_Warning[];   // 警告图标（在OLED_Font.h中定义）

uint16_t distance_cm = 0;             // 前方障碍物距离（cm）
int16_t AccX = 0, AccY = 0, AccZ = 0; // MPU6050三轴加速度原始值

/**
  * @brief  刷新数据：读取MPU6050加速度和超声波距离，更新到OLED
  * @param  无
  * @retval 无
  */
void Refresh_Display(void)
{
	/* 读取加速度计数据（陀螺仪参数传NULL不读取） */
	MPU6050_GetData(&AccX, &AccY, &AccZ, NULL, NULL, NULL);
	OLED_ShowSignedNum(2, 3, AccX, 5);    // 第2行显示X轴加速度
	OLED_ShowSignedNum(3, 3, AccY, 5);    // 第3行显示Y轴加速度
	OLED_ShowSignedNum(4, 3, AccZ, 5);    // 第4行显示Z轴加速度

	/* 读取超声波距离 */
	distance_cm = HCSR04_GetValue();
	OLED_ShowNum(1, 6, distance_cm, 3);  // 第1行第6列显示距离数值
}

/**
  * @brief  绘制完整显示界面：清屏后打印X/Y/Z加速度标签和距离汉字
  * @param  无
  * @retval 无
  */
void Display(void)
{
	OLED_Clear();

	/* X轴加速度 */
	OLED_ShowString(2, 1, "X:");
	OLED_ShowSignedNum(2, 3, AccX, 5);

	/* Y轴加速度 */
	OLED_ShowString(3, 1, "Y:");
	OLED_ShowSignedNum(3, 3, AccY, 5);

	/* Z轴加速度 */
	OLED_ShowString(4, 1, "Z:");
	OLED_ShowSignedNum(4, 3, AccZ, 5);

	/* 第1行：显示"距离:"两个汉字+数值+单位cm */
	OLED_ShowCN(1, 1, 11, 1);   // "距"
	OLED_ShowCN(1, 2, 12, 1);   // "离"
	OLED_ShowChar(1, 5, ':');
	OLED_ShowNum(1, 6, distance_cm, 3);
	OLED_ShowString(1, 9, "cm");
}

/**
  * @brief  SOS声光报警：OLED显示SOS汉字+警告图标，LED交替闪烁，蜂鸣器鸣响
  * @note   循环5次，每次亮500ms灭500ms
  * @param  无
  * @retval 无
  */
void SOS(void)
{
	for (int i = 0; i < 5; i++){
		/* 亮相位：显示"SOS警告"汉字，LED1亮，蜂鸣器响 */
		OLED_Clear();
		OLED_ShowString(2, 5, "SOS");
		OLED_ShowCN(2, 5, 3, 1);   // "警"
		OLED_ShowCN(2, 6, 2, 1);   // "告"
		LED1_ON();
		LED2_OFF();
		Buzzer_ON();
		Delay_ms(500);

		/* 灭相位：显示警告位图，LED2亮，蜂鸣器停 */
		OLED_Clear();
		OLED_ShowBMP(32, 0, 64, 64, BMP_Warning, 1);
		LED1_OFF();
		LED2_ON();
		Buzzer_OFF();
		Delay_ms(500);

		LED2_OFF();
		OLED_Clear();
	}
}

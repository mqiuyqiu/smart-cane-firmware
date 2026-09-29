/**
  ******************************************************************************
  * @file    main.c
  * @brief   基于STM32F103的智能拐杖主程序
  * @details 功能模块：
  *           - HC-SR04超声波避障：前方障碍物<20cm时LED闪烁+语音提示
  *           - SOS一键求救：长按PB1按键2秒触发声光报警+语音播报
  *           - MPU6050跌倒检测：合加速度+姿态角双重判定，跌倒后蜂鸣器报警
  *           - OLED实时显示距离与三轴加速度
  *           - JQ8900语音模块：IO口触发播放预置语音
  *
  * 引脚分配：
  *   PA6  - LED1 警示灯（低电平点亮）
  *   PA7  - LED2 警示灯（低电平点亮）
  *   PB0  - HC-SR04 Trig  超声波触发输出
  *   PB1  - SOS按键        外部中断EXTI1（上拉输入，长按2秒触发）
  *   PB5  - HC-SR04 Echo  超声波回声输入（Timer2计数脉宽）
  *   PB8  - OLED_SCL      软件I2C时钟
  *   PB9  - OLED_SDA      软件I2C数据
  *   PB10 - MPU6050_SCL   软件I2C时钟
  *   PB11 - MPU6050_SDA   软件I2C数据
  *   PB12 - Buzzer 蜂鸣器  低电平鸣响
  *   PB13 - JQ8900_IO1   触发"距离过近"语音
  *   PB14 - JQ8900_IO2   触发"SOS报警"语音
  *   PB15 - JQ8900_IO3   触发"跌倒报警"语音
  *
  * @author  智能拐杖项目组
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "OLED.h"
#include "Delay.h"
#include "Key.h"
#include "LED.h"
#include "Buzzer.h"
#include "Display.h"
#include "HC_SR04.h"
#include "Timer.h"
#include "JQ8900.h"
#include "MPU6050.h"
#include "MPU6050_FallDetect.h"

extern const uint8_t BMP_Welcome[];   // 开机欢迎位图（在OLED_Font.h中定义）

uint8_t sos_trigger_flag = 0;          // SOS报警标志：1=正在报警中，期间屏蔽距离预警

/**
  * @brief  主函数：初始化各外设后进入主循环
  * @param  无
  * @retval int
  */
int main(void)
{
	/* ---------- 外设初始化 ---------- */
	SysTick_Config(SystemCoreClock / 1000);  // 配置SysTick为1ms中断，提供系统时基
	Timer2_Init();                           // 启动Timer2，用于超声波Echo脉宽计时
	Timer3_Init();                           // Timer3初始化（系统节拍/预留）
	SOSKey_Init();                           // SOS按键：EXTI外部中断+TIM4长按检测
	OLED_Init();                             // OLED屏幕初始化
	LED_Init();                              // 两路警示灯GPIO
	Buzzer_Init();                           // 蜂鸣器GPIO
	HCSR04_Init();                           // 超声波Trig/Echo引脚
	JQ8900_Init();                           // 语音模块3路IO触发引脚
	MPU6050_Init();                          // 六轴传感器（软件I2C）

	/* ---------- 开机画面 ---------- */
	OLED_ShowBMP(0, 0, 128, 64, BMP_Welcome, 1);  // 显示开机欢迎图
	Delay_s(2);                                    // 停留2秒

	Display();                                     // 刷新为正常测量界面

	/* ---------- 主循环 ---------- */
	while(1)
	{
		/* 1. 超声波测距并刷新OLED显示 */
		uint16_t distance = HCSR04_GetValue();     // 获取前方距离（cm）
		Refresh_Display();                         // 刷新OLED上的加速度和距离数值

		/* 2. 前方障碍预警：距离<=20cm且当前不在SOS报警中 */
		if(distance <= 20 && !sos_trigger_flag)
		{
			LED1_ON();
			LED2_ON();
			JQ8900_Play_IO1();                     // 播放"前方障碍物"语音
		}
		else
		{
			LED1_OFF();
			LED2_OFF();
		}

		/* 3. SOS按键：长按2秒后Key_GetState()返回1 */
		if(Key_GetState())
		{
			sos_trigger_flag = 1;                  // 进入报警状态，屏蔽距离预警
			JQ8900_Play_IO2();                    // 播放"SOS求救"语音
			SOS();                                // 执行声光报警闪烁
			Display();                            // 报警结束后恢复正常界面
		}
		else
		{
			sos_trigger_flag = 0;
		}

		/* 4. 跌倒检测：MPU6050连续异常后判定跌倒 */
		if(MPU6050_CheckFall() == FALL_STATE_DETECTED)
		{
			JQ8900_Play_IO3();                    // 播放"跌倒提醒"语音
			Buzzer_ON();                          // 蜂鸣器长鸣
			Delay_s(2);                           // 持续报警2秒
		}
		else
		{
			Buzzer_OFF();
		}

//		__WFI();   // 进入低功耗停机模式（电池供电时取消注释可省电）
	}
}

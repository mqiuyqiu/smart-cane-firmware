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

extern const uint8_t BMP_Welcome[];

uint8_t sos_trigger_flag = 0; 

int main(void)
{
	//初始化 
	SysTick_Config(SystemCoreClock / 1000); 
	Timer2_Init();
	SOSKey_Init();
	OLED_Init();
	LED_Init();
	Buzzer_Init();
	HCSR04_Init();
	JQ8900_Init();
	MPU6050_Init();
	
	OLED_ShowBMP(0, 0, 128, 64, BMP_Welcome,1);//显示欢迎图像
	Delay_s(2);
	
	Display();//初始化显示
	
	while(1)
	{		
		uint16_t distance = HCSR04_GetValue(); 
		Refresh_Display();	
		if(distance <= 20 && !sos_trigger_flag)
		{
			LED1_ON();
			LED2_ON();
			JQ8900_Play_IO1();  // 播放“距离过近”语音
		}
		else
		{
			LED1_OFF();
			LED2_OFF();
		}
		if(Key_GetState())//判断sos摁键是否触发
		{
			sos_trigger_flag = 1;
			JQ8900_Play_IO2();// 播放“SOS报警”语音
			SOS();//警告函数，声光提醒加上传记录
			Display();//重新初始化显示
		}
		else
		{
			sos_trigger_flag = 0;
		}
		 if(MPU6050_CheckFall() == FALL_STATE_DETECTED)
		 {
			 JQ8900_Play_IO3();
			 Buzzer_ON();
			 Delay_s(2);
		 }
		 else
		 {
			 Buzzer_OFF();
		 }
			 
		 
//		__WFI();	//适配电池
	}
}

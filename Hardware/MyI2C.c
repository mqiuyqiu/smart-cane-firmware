/**
  ******************************************************************************
  * @file    MyI2C.c
  * @brief   软件模拟I2C通信驱动（用于MPU6050）
  * @details SCL接PB10，SDA接PB11，开漏输出。
  *          参考江协科技教学代码，通过GPIO位模拟I2C起始/停止/收发。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * @brief  I2C写SCL线
  * @param  BitValue: 0=低电平，1=高电平
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);
	Delay_us(10);    // 时序间隔
}

/**
  * @brief  I2C写SDA线
  * @param  BitValue: 0=低电平，1=高电平
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);
	Delay_us(10);
}

/**
  * @brief  I2C读SDA线
  * @retval 0=低电平，1=高电平
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Delay_us(10);
	return BitValue;
}

/**
  * @brief  初始化I2C引脚：PB10/PB11开漏输出，默认高电平
  * @param  无
  * @retval 无
  */
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    // 开漏输出（I2C必须）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

/**
  * @brief  I2C起始信号：SDA高电平期间SCL由高变低
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

/**
  * @brief  I2C停止信号：SCL高电平期间SDA由低变高
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节（MSB先发）
  * @param  Byte: 要发送的字节
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));   // 从最高位开始逐位输出
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

/**
  * @brief  I2C接收一个字节（MSB先收）
  * @retval 收到的字节
  */
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);    // 释放SDA（接收模式）
	for (i = 0; i < 8; i++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

/**
  * @brief  I2C发送应答位
  * @param  AckBit: 0=ACK应答，1=NACK非应答
  */
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

/**
  * @brief  I2C接收应答位
  * @retval 0=从机ACK，1=从机NACK
  */
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}

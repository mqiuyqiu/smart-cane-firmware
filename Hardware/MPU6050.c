/**
  ******************************************************************************
  * @file    MPU6050.c
  * @brief   MPU6050六轴加速度陀螺仪传感器驱动
  * @details 通过软件I2C（MyI2C）通信，7位地址0x68（8位写地址0xD0）。
  *          配置±16g加速度量程、±2000dps陀螺仪量程，采样率100Hz。
  *          参考江协科技教学代码。
  ******************************************************************************
  */

#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0        // MPU6050写地址（7位地址0x68<<1 | 0）

/**
  * @brief  向MPU6050指定寄存器写一个字节
  * @param  RegAddress: 寄存器地址
  * @param  Data: 要写入的数据
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

/**
  * @brief  从MPU6050指定寄存器读一个字节
  * @param  RegAddress: 寄存器地址
  * @retval 读到的数据
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;

	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();

	/* 重复起始条件：切换为读模式 */
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);   // 读地址（最低位置1）
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);                          // NACK：只读一个字节
	MyI2C_Stop();

	return Data;
}

/**
  * @brief  初始化MPU6050：配置时钟、采样率、量程
  * @note   加速度±16g(0x18)，陀螺仪±2000dps(0x18)，采样率100Hz
  * @param  无
  * @retval 无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);   // 时钟源选择X陀螺PLL，解除休眠
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);   // 加速度+陀螺仪全部使能
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);   // 采样率=1kHz/(1+9)=100Hz
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);       // 低通滤波DLPF=5（带宽~10Hz）
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);  // 陀螺仪量程±2000dps
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);  // 加速度量程±16g
}

/**
  * @brief  读取MPU6050的WHO_AM_I ID寄存器（应返回0x68）
  * @retval 芯片ID
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

/**
  * @brief  读取三轴加速度和陀螺仪原始数据
  * @param  AccX/AccY/AccZ: 三轴加速度原始值指针（传NULL不读取）
  * @param  GyroX/GyroY/GyroZ: 三轴陀螺仪原始值指针（传NULL不读取）
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;

	/* 加速度X轴 */
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;

	/* 加速度Y轴 */
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;

	/* 加速度Z轴 */
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;

	/* 陀螺仪X轴 */
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;

	/* 陀螺仪Y轴 */
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;

	/* 陀螺仪Z轴 */
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

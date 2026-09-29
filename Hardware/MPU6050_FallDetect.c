/**
  ******************************************************************************
  * @file    MPU6050_FallDetect.c
  * @brief   基于MPU6050的跌倒检测算法
  * @details 双重判定策略：
  *          1. 合加速度判据：|a_total| 超出 [0.8g, 1.2g] 认为异常（跌倒冲击或失重）
  *          2. 姿态角判据：Pitch/Roll 超过45°认为倒地姿态
  *          3. 防抖：连续3次（约300ms）异常才判定为跌倒，避免误触发
  *
  * 阈值可调（在MPU6050_FallDetect.h中）：
  *   ACC_NORMAL_MIN/MAX  合加速度正常范围(g)
  *   ANGLE_FALL_THRESH   姿态角阈值(°)
  *   FALL_COUNT_THRESH   连续异常次数
  ******************************************************************************
  */

#include "MPU6050_FallDetect.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

/* 全局防抖计数（静态变量，仅本文件可见） */
static uint8_t fall_detect_count = 0;

/**
  * @brief  弧度转角度
  * @param  rad: 弧度值
  * @retval 角度值
  */
static float Rad2Deg(float rad)
{
    return rad * 180.0f / M_PI;
}

/**
  * @brief  计算合加速度（单位：g）
  * @param  acc_x/y/z: MPU6050原始加速度值
  * @retval 合加速度幅值(g)，静止直立时约1.0g
  */
static float MPU6050_CalcAccTotal(int16_t acc_x, int16_t acc_y, int16_t acc_z)
{
    /* 原始值转g：±16g量程 -> 1LSB = 16/32768 = 1/2048 g */
    float ax = (float)acc_x / 2048.0f;
    float ay = (float)acc_y / 2048.0f;
    float az = (float)acc_z / 2048.0f;

    /* 合加速度 = sqrt(ax^2 + ay^2 + az^2) */
    return sqrt(ax*ax + ay*ay + az*az);
}

/**
  * @brief  由加速度计计算静态姿态角（俯仰角/翻滚角）
  * @param  acc_x/y/z: 三轴加速度原始值
  * @param  pitch: 输出俯仰角(前后倾斜)，单位°
  * @param  roll: 输出翻滚角(左右倾斜)，单位°
  */
static void MPU6050_CalcAngle(int16_t acc_x, int16_t acc_y, int16_t acc_z, float *pitch, float *roll)
{
    float ax = (float)acc_x / 2048.0f;
    float ay = (float)acc_y / 2048.0f;
    float az = (float)acc_z / 2048.0f;

    /* 俯仰角（前后倾斜）：Pitch = arctan(ay / sqrt(ax^2 + az^2)) */
    *pitch = Rad2Deg(atan2(ay, sqrt(ax*ax + az*az)));

    /* 翻滚角（左右倾斜）：Roll = arctan(ax / sqrt(ay^2 + az^2)) */
    *roll = Rad2Deg(atan2(ax, sqrt(ay*ay + az*az)));
}

/**
  * @brief  跌倒检测核心函数（主循环周期调用）
  * @retval 1=检测到跌倒，0=正常
  * @note   需在主循环中定期调用（建议每100ms一次）
  */
uint8_t MPU6050_CheckFall(void)
{
    int16_t acc_x, acc_y, acc_z;
    int16_t gyro_x, gyro_y, gyro_z;   // 陀螺仪暂未使用，预留扩展
    float acc_total;
    float pitch, roll;
    uint8_t is_abnormal = 0;

    /* 1. 读取MPU6050原始数据 */
    MPU6050_GetData(&acc_x, &acc_y, &acc_z, &gyro_x, &gyro_y, &gyro_z);

    /* 2. 合加速度异常时进一步判断姿态角 */
    acc_total = MPU6050_CalcAccTotal(acc_x, acc_y, acc_z);
    if(acc_total < ACC_NORMAL_MIN || acc_total > ACC_NORMAL_MAX)
    {
        MPU6050_CalcAngle(acc_x, acc_y, acc_z, &pitch, &roll);

        /* 姿态角超过阈值 -> 判定为异常状态 */
        if(fabs(pitch) > ANGLE_FALL_THRESH || fabs(roll) > ANGLE_FALL_THRESH)
        {
            is_abnormal = 1;
        }
    }

    /* 3. 防抖过滤：连续异常才判定跌倒 */
    if(is_abnormal)
    {
        fall_detect_count++;
        if(fall_detect_count >= FALL_COUNT_THRESH)
        {
            return FALL_STATE_DETECTED;
        }
    }
    else
    {
        fall_detect_count = 0;    // 无异常立即清零
    }

    return FALL_STATE_NORMAL;
}

/**
  * @brief  复位跌倒检测状态（报警结束后调用）
  */
void MPU6050_ResetFallState(void)
{
    fall_detect_count = 0;
}

#include "MPU6050_FallDetect.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

// 全局防抖计数（静态变量，仅本文件可见）
static uint8_t fall_detect_count = 0;

// 内部辅助函数
// 弧度转角度
static float Rad2Deg(float rad)
{
    return rad * 180.0f / M_PI;
}

// 计算合加速度（单位：g）
static float MPU6050_CalcAccTotal(int16_t acc_x, int16_t acc_y, int16_t acc_z)
{
    // 原始值转g：±16g量程 → 1LSB = 16/32768 = 1/2048 g
    float ax = (float)acc_x / 2048.0f;
    float ay = (float)acc_y / 2048.0f;
    float az = (float)acc_z / 2048.0f;
    
    // 合加速度 = √(ax² + ay² + az²)
    return sqrt(ax*ax + ay*ay + az*az);
}

// 计算静态姿态角（Pitch：俯仰角，Roll：翻滚角，单位：°）
static void MPU6050_CalcAngle(int16_t acc_x, int16_t acc_y, int16_t acc_z, float *pitch, float *roll)
{
    // 原始值转g
    float ax = (float)acc_x / 2048.0f;
    float ay = (float)acc_y / 2048.0f;
    float az = (float)acc_z / 2048.0f;
    
    // 俯仰角（前后倾斜）：Pitch = arctan(ay / √(ax² + az²))
    *pitch = Rad2Deg(atan2(ay, sqrt(ax*ax + az*az)));
    
    // 翻滚角（左右倾斜）：Roll = arctan(ax / √(ay² + az²))
    *roll = Rad2Deg(atan2(ax, sqrt(ay*ay + az*az)));
}

// 对外核心函数
// 检测是否跌倒（返回1=跌倒，0=正常）
uint8_t MPU6050_CheckFall(void)
{
    int16_t acc_x, acc_y, acc_z;
    int16_t gyro_x, gyro_y, gyro_z; // 陀螺仪暂未用到，可留作扩展
    float acc_total;
    float pitch, roll;
    uint8_t is_abnormal = 0;

    // 1. 读取MPU6050原始数据
    MPU6050_GetData(&acc_x, &acc_y, &acc_z, &gyro_x, &gyro_y, &gyro_z);
    
    // 2. 计算合加速度，判断是否异常
    acc_total = MPU6050_CalcAccTotal(acc_x, acc_y, acc_z);
    if(acc_total < ACC_NORMAL_MIN || acc_total > ACC_NORMAL_MAX)
    {
        // 合加速度异常 → 进一步判断姿态角
        MPU6050_CalcAngle(acc_x, acc_y, acc_z, &pitch, &roll);
        
        // 姿态角超过阈值 → 判定为异常状态
        if(fabs(pitch) > ANGLE_FALL_THRESH || fabs(roll) > ANGLE_FALL_THRESH)
        {
            is_abnormal = 1;
        }
    }
    
    // 3. 防抖过滤：连续异常才判定跌倒
    if(is_abnormal)
    {
        fall_detect_count++;
        // 连续异常达到阈值 → 判定跌倒
        if(fall_detect_count >= FALL_COUNT_THRESH)
        {
            return FALL_STATE_DETECTED; // 返回1（跌倒）
        }
    }
    else
    {
        // 无异常 → 重置计数
        fall_detect_count = 0;
    }
    
    return FALL_STATE_NORMAL; // 返回0（正常）
}

// 复位跌倒检测状态（报警后调用，重置防抖计数）
void MPU6050_ResetFallState(void)
{
    fall_detect_count = 0;
}

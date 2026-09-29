#ifndef __MPU6050_FALLDETECT_H
#define __MPU6050_FALLDETECT_H

#include "stdint.h"
#include "math.h"
#include "MPU6050.h" // 包含你的原有MPU6050驱动头文件

// 阈值调整区
#define ACC_NORMAL_MIN    0.9f    // 合加速度正常最小值（g）
#define ACC_NORMAL_MAX    1.1f    // 合加速度正常最大值（g）
#define ANGLE_FALL_THRESH 45.0f   // 跌倒姿态角阈值（°），超过则判定姿态异常
#define FALL_COUNT_THRESH 3       // 防抖计数阈值（每次检测间隔100ms）

// 跌倒状态枚举
typedef enum {
    FALL_STATE_NORMAL = 0, // 正常
    FALL_STATE_DETECTED    // 跌倒
} Fall_State;

// 核心函数：检测是否跌倒（返回1=跌倒，0=正常）
uint8_t MPU6050_CheckFall(void);

void MPU6050_ResetFallState(void);

#endif

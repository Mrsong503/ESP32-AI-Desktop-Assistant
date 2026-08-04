#pragma once

#include <stdint.h>
#include <stddef.h>

//======================================
// INMP441 I2S 数字麦克风
// SCK  -> GPIO4
// WS   -> GPIO5
// SD   -> GPIO6
// L/R  -> GND (左声道)
//======================================

class Mic
{
public:
    bool begin();
    void end();

    // 读取麦克风采样数据（32-bit 帧），返回实际读取的样本数
    size_t read(int32_t* buffer, size_t samples);

    // 读取并返回当前音量等级 (0.0 ~ 1.0)
    float getLevel();

    // 读取并返回当前峰值 (0 ~ 1000)
    int getPeak();

private:
    bool initialized_ = false;
    float computeLevel(const int32_t* buffer, size_t samples);
};
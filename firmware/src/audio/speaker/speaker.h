#pragma once

#include <stdint.h>
#include <stddef.h>

//======================================
// MAX98357A I2S 数字功放
// BCLK -> GPIO15
// LRC  -> GPIO16
// DIN  -> GPIO17
// SD   -> 3.3V (使能)
//======================================

class Speaker
{
public:
    bool begin();
    void end();

    // 播放 PWM 测试音
    void playTestTone(float frequency = 1000.0f, uint32_t durationMs = 200);

    // 写入音频数据（16-bit）
    size_t write(const int16_t* data, size_t samples);

private:
    bool initialized_ = false;
};
#pragma once

#include "audio/mic/mic.h"
#include "audio/speaker/speaker.h"

//======================================
// Audio 模块（统一入口）
// 初始化 I2S 麦克风 + I2S 功放
//======================================

class Audio
{
public:
    bool begin();
    void end();

    Mic&     mic();
    Speaker& speaker();

    // 播放测试音（确认功放正常）
    void playTestTone(float frequency = 1000.0f, uint32_t durationMs = 300);

    // 录制测试（读取麦克风并打印音量，确认麦克风正常）
    void recordTest(uint32_t durationMs = 2000);

private:
    Mic mic_;
    Speaker speaker_;
};
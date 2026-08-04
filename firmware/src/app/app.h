#pragma once

#include "audio/audio.h"

//======================================
// 应用主控制器
// 负责模块生命周期管理
//======================================

class App
{
public:
    bool begin();
    void update();

    Audio& audio();

private:
    Audio audio_;
};
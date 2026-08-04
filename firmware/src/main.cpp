#include <Arduino.h>
#include <HWCDC.h>

#include "app/app.h"

//======================================
// Eliya's Mind 入口
// main.cpp 只负责生命周期，不写业务逻辑
//======================================

App app;

void setup()
{
    USBSerial.begin(115200);
    delay(100);

    app.begin();

    // ---- Audio 自检流程 ----
    USBSerial.println("\n=== Audio Test ===");

    // 1. 播放测试音（确认功放/喇叭正常）
    app.audio().playTestTone(1000, 200);
    delay(200);
    app.audio().playTestTone(2000, 200);

    // 2. 麦克风录音测试（确认麦克风正常，串口打印音量）
    delay(500);
    app.audio().recordTest(3000);
}

void loop()
{
    app.update();
}
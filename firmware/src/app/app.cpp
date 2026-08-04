#include "app.h"

#include <Arduino.h>
#include <HWCDC.h>

bool App::begin()
{
    USBSerial.println("========================================");
    USBSerial.println("  Eliya's Mind v0.1.0");
    USBSerial.println("========================================");

    bool ok = true;

    if (!audio_.begin())
    {
        USBSerial.println("[App] Audio init FAILED");
        ok = false;
    }
    else
    {
        USBSerial.println("[App] Audio init OK");
    }

    return ok;
}

void App::update()
{
    // 预留：后续在这里做轮询/事件处理
    // 例如每隔一段时间读取 AHT20 并发布到事件总线
}

Audio& App::audio()
{
    return audio_;
}
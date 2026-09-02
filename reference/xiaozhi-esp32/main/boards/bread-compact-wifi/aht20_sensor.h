#pragma once

#include <stdbool.h>
#include <stdint.h>

//======================================
// AHT20 温湿度传感器
// I2C 地址: 0x38
// SDA -> GPIO8, SCL -> GPIO9
// 通信方式: GPIO bit-bang（绕过 ESP-IDF i2c_master 驱动）
//   原因: IDF 5.x i2c_master 在 GPIO8/9 上全部数据 NACK，
//         但 GPIO bit-bang 同硬件能正常通信（已用诊断日志确认）。
//======================================

class AHT20Sensor
{
public:
    AHT20Sensor();
    ~AHT20Sensor();

    // bit-bang 直连 GPIO，无需 I2C 总线句柄
    bool begin();

    // 触发一次测量并读取数据
    bool update();

    // 获取温度 (°C)
    float getTemperature() const;

    // 获取湿度 (%)
    float getHumidity() const;

    // 传感器是否可用 (已正确初始化并校准)
    bool isAvailable() const;

private:
    // 软复位
    bool softReset();

    // 初始化校准
    bool initSensor();

    // 检查校准状态
    bool checkCalibration();

    // 等待测量完成 (轮询忙标志位)
    bool waitForMeasurement(uint32_t timeoutMs = 50);

    bool initialized_;
    bool available_;
    float temperature_;
    float humidity_;
};
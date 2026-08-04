# 🔌 硬件文档

## 开发板

**ESP32-S3-DevKitC-1**（N8，8MB Flash，无 PSRAM）

## 外设接线表

| 外设 | 接口 | 引脚 | 说明 |
|------|------|------|------|
| **AHT20** 温湿度 | I2C | SDA→GPIO8, SCL→GPIO9 | VIN, GND |
| **INMP441** 麦克风 | I2S | SCK→GPIO4, WS→GPIO5, SD→GPIO6, L/R→GND | VDD 3.3V, GND |
| **MAX98357A** 功放 | I2S | BCLK→GPIO15, LRC→GPIO16, DIN→GPIO17, SD→3.3V | VIN 3.3V, GND, GAIN 悬空 |
| **喇叭** 4Ω 3W | - | 接 MAX98357A 输出 | 型号 3525 |
| **RGB LED** | 板载 | GPIO48 | NeoPixel 1 颗 |
| **BOOT 按钮** | 板载 | GPIO0 | - |

## 未来计划购买

- **4.0 寸 ST7796 SPI IPS 电容触摸屏**（若有更好的 ESP32 兼容屏可推荐）
- **红外发射模块**：控制空调/电视/风扇等红外家电
- **3.5mm 耳机接口**：自动检测插入切换输出

## GPIO 总览（集中管理，便于画 PCB）

| GPIO | 功能 |
|------|------|
| 0 | BOOT 按钮 |
| 4 | I2S 麦克风 SCK |
| 5 | I2S 麦克风 WS |
| 6 | I2S 麦克风 SD |
| 8 | I2C SDA（AHT20） |
| 9 | I2C SCL（AHT20） |
| 15 | I2S 功放 BCLK |
| 16 | I2S 功放 LRC |
| 17 | I2S 功放 DIN |
| 48 | RGB LED |

## 注意事项

- MAX98357A 的 `SD`（关断）引脚接 3.3V 使能输出
- INMP441 的 `L/R` 接 GND 表示左声道
- 所有传感器电源统一 3.3V，注意总电流不要超过 LDO 能力
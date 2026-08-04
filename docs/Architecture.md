# 🏗️ 软件架构

> 原则：**所有模块之间不直接互相调用，全部通过 Event Bus（事件总线）通信。**

## 整体架构

```
              Eliya's Mind
                    │
        ┌───────────┴───────────┐
     Voice Core             UI Core
        │                       │
   (小智 AI)                 (Screen)
        │                       │
        └──── Event Bus ────────┘
                    │
   ┌───────┬───────┬───────┬────────┬──────┐
  MQTT   Sensor  Music   Todo     HA    OTA
   │       │       │       │        │      │
 Weather  AHT20  Alarm  Calendar  Plugin WebConfig
```

## 模块划分（firmware/src）

```
src/
├── main.cpp          # 仅入口：setup() + app.begin() + loop() + app.update()
├── app/              # 应用主控制器
├── audio/            # 音频模块（I2S 麦克风 + 功放）
│   ├── mic/          # 麦克风（INMP441）
│   └── speaker/      # 扬声器（MAX98357A）
├── display/          # 屏幕 UI
├── sensor/           # 传感器（AHT20）
├── assistant/        # 语音助手核心
├── mqtt/             # MQTT 通信
├── wifi/             # WiFi 管理
├── homeassistant/    # Home Assistant 集成
├── eventbus/         # 事件总线
├── config/           # 配置管理
└── utils/            # 通用工具
```

## 通信原则

- 模块间**禁止直接调用**
- 所有事件通过 `EventBus` 发布/订阅
- 例如：
  - `AHT20` 发布 → `sensor.temperature.changed`
  - `Music` 订阅 → `music.play`
  - `HomeAssistant` 订阅 → `light.turn_on`

## 代码风格

- C++ 类封装
- 尽量避免全局变量
- 所有 GPIO 集中管理（便于画 PCB）
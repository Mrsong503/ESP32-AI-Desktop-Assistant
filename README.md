# 🧠 Eliya's Mind

一个独立运行、具有语音交互能力、可以管理生活和智能家居的**桌面 AI Assistant**。

> 不是一个 ESP32 Demo，而是一个长期维护的产品。

## ✨ 特性

- 🎙️ 语音交互（小智 AI / 自研）
- 🗣️ 唤醒词 + 连续对话
- 📶 MQTT 协议
- 🏠 Home Assistant 集成
- 🌡️ AHT20 温湿度传感器
- ⛅ 天气查询
- ✅ Todo 待办
- 🎵 音乐播放
- 🔄 OTA 无线升级
- 🌐 Web 配置页面
- 📺 屏幕 UI
- 🕹️ 红外控制家电（空调/电视/风扇）
- 🔌 后续自制 PCB

## 📁 工程结构

```
Eliya's Mind/
├── firmware/              # ESP32 固件（PlatformIO + Arduino）
├── server/                # 后端服务（Python，规划中）
├── app/                   # PWA 前端（规划中）
├── reference/             # 参考项目（小智 xiaozhi-esp32）
├── docs/                  # 文档
│   ├── Architecture.md
│   └── Hardware.md
├── assets/                # 资源文件
└── README.md
```

## 🔧 开发环境

- Windows + VSCode + PlatformIO
- ESP32-S3-DevKitC-1
- Arduino Framework

## 🚀 快速开始

1. 用 VSCode 打开 `firmware/` 目录
2. 编译：`PlatformIO: Build`
3. 烧录：`PlatformIO: Upload`
4. 串口监视器：`PlatformIO: Monitor`（115200）

## 📍 当前阶段

- [x] AHT20 温湿度读取
- [x] ThingSpeak 云平台推送
- [x] ESP32 本地 Web 仪表盘
- [ ] Audio 模块（I2S 麦克风 + 功放）——当前进行中
- [ ] 小智 AI 语音接入
- [ ] 后续功能...

## 📜 版本

- v0.1.0 初始化工程与 Audio 模块
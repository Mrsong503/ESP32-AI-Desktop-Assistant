# 🧠 Eliya's Mind

一个独立运行、具有语音交互能力、可以管理生活和智能家居的**桌面 AI Assistant**。

> 不是一个 ESP32 Demo，而是一个长期维护的产品。

## ✨ 特性

- 🎙️ 语音交互（小智 AI）
- 🗣️ 唤醒词（"你好小智"）+ 连续对话
- 🌡️ AHT20 温湿度传感器（语音查询温度/湿度）
- 📶 MQTT / WebSocket 协议
- 🏠 Home Assistant 集成（规划中）
- ⛅ 天气查询（规划中）
- ✅ Todo 待办（规划中）
- 🎵 音乐播放（规划中）
- 🔄 OTA 无线升级
- 🌐 Web 配置页面
- 📺 屏幕 UI（规划中）
- 🕹️ 红外控制家电（规划中）
- 🔌 后续自制 PCB

## 📁 工程结构

```
Eliya's Mind/
├── firmware/              # ESP32 固件
│   ├── src/               # 自研 PlatformIO 工程（Arduino）
│   ├── main/              # 小智 xiaozhi-esp32 ESP-IDF 工程
│   ├── platformio.ini     # PlatformIO 配置
│   └── CMakeLists.txt     # ESP-IDF 配置
├── reference/             # 小智 xiaozhi-esp32 官方源码（实际编译烧录用）
│   └── xiaozhi-esp32/     # ESP-IDF 工程（含 bread-compact-wifi 板级定制 + AHT20 驱动）
├── server/                # 后端服务（Python，规划中）
├── app/                   # PWA 前端（规划中）
├── docs/                  # 文档
│   ├── Architecture.md
│   └── Hardware.md
├── PROGRESS.md            # 项目进度记录
└── README.md
```

## 🔧 开发环境

- Windows + VSCode
- ESP32-S3-DevKitC-1（N8，8MB Flash，无 PSRAM）
- ESP-IDF v5.5.5（小智固件）
- PlatformIO + Arduino（自研工程）

## 🚀 快速开始

### 小智固件（ESP-IDF，实际编译烧录使用）

```bash
cd reference/xiaozhi-esp32
python scripts/build.py --board bread-compact-wifi
```

> 注意：Windows 项目路径若含 Unicode 字符，可使用 `scripts/build_aht20.py`（自动复制到 ASCII 路径编译）。

### 自研工程（PlatformIO）

1. 用 VSCode 打开 `firmware/` 目录
2. 编译：`PlatformIO: Build`
3. 烧录：`PlatformIO: Upload`
4. 串口监视器：`PlatformIO: Monitor`（115200）

## 📍 当前阶段

- [x] AHT20 温湿度读取
- [x] Audio 模块（I2S 麦克风 INMP441 + 功放 MAX98357A）
- [x] 小智 AI 语音接入（ESP-IDF 固件）
- [x] 唤醒词 + 语音对话
- [x] 语音查询温湿度（AHT20 MCP Tool）
- [ ] 后续功能...

## 📜 版本

- v0.1.0 初始化工程与 Audio 模块
- v0.2.0 小智 AI 语音接入（ESP-IDF）
- v0.3.0 AHT20 温湿度语音查询（MCP Tool）

## 🔌 硬件接线

| 外设 | 接口 | 引脚 |
|------|------|------|
| **AHT20** 温湿度 | I2C | SDA→GPIO8, SCL→GPIO9 |
| **INMP441** 麦克风 | I2S | SCK→GPIO4, WS→GPIO5, SD→GPIO6 |
| **MAX98357A** 功放 | I2S | BCLK→GPIO15, LRC→GPIO16, DIN→GPIO17 |
| **RGB LED** | 板载 | GPIO48 |
| **BOOT 按钮** | 板载 | GPIO0 |

详见 [docs/Hardware.md](docs/Hardware.md)
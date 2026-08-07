# 🏗️ 软件架构

> 当前采用**小智 xiaozhi-esp32（ESP-IDF）** 作为语音核心，MCP (Model Context Protocol) 作为设备能力扩展框架。

## 整体架构

```
               Eliya's Mind
                     │
          ┌──────────┴──────────┐
      Voice Core            Device Capabilities
          │                    (MCP Tools)
   (小智 xiaozhi-esp32)             │
          │                    ┌────┴────┐
          │                 Sensors   Actuators
          │                 (AHT20)    (LED等)
          │                        │
          └──── MCP 协议 ──────────┘
                     │
         ┌───────────┴───────────┐
       WebSocket / MQTT        OTA / 配网
```

## Voice Core（小智 xiaozhi-esp32）

**目录：** `firmware/main/`（ESP-IDF 工程）

```
main/
├── application.cc        # 语音状态机、协议处理（不修改）
├── mcp_server.cc         # MCP 服务器：工具注册/调用/JSON-RPC（不修改）
├── mcp_server.h          # MCP 框架：McpTool/McpServer/Property（不修改）
├── boards/
│   └── bread-compact-wifi/
│       ├── compact_wifi_board.cc   # 板级初始化 + 自定义 MCP 工具注册
│       ├── config.h                # 引脚配置
│       └── aht20_sensor.*          # AHT20 驱动（新增）
├── audio/                # 音频管线（INMP441 + MAX98357A）
├── protocols/            # WebSocket / MQTT 协议
└── display/              # OLED / LCD 显示
```

## MCP 工具扩展机制

**核心原则：自定义工具必须在板级 `InitializeTools()` 中注册，不得修改 MCP 框架本身。**

```
用户语音 "现在温度多少？"
    │
    ▼
云端 ASR + LLM ── 判定需要传感器数据
    │
    ▼
云端发送 MCP tools/call {"type":"mcp", ...}
    │
    ▼
设备 application.cc 收到 → McpServer::ParseMessage()
    │
    ▼
DoToolCall("self.sensor.get_temperature_humidity")
    │
    ▼
回调函数读取 AHT20 → 返回 JSON
    │
    ▼
LLM 组织回答 → TTS → 扬声器播放
```

## 自研 PlatformIO 工程（规划中）

**目录：** `firmware/src/`（Arduino）

```
src/
├── main.cpp          # 入口：setup() + loop()
├── app/              # 应用主控制器
├── audio/            # 音频模块（I2S 麦克风 + 功放）
│   ├── mic/          # 麦克风（INMP441）
│   └── speaker/      # 扬声器（MAX98357A）
└── config/           # GPIO 引脚集中管理（pins.h）
```

## 通信原则

- **语音链路**：完全依赖小智官方 Voice Core（ASR/LLM/TTS/唤醒词），不自行实现
- **设备能力**：通过 MCP Tool 机制暴露，AI 按需调用
- **数据格式**：MCP Tool 返回 JSON（结构化数据）给 LLM 组织自然语言回答

## 代码风格

- C++ 类封装（ESP-IDF 风格）
- 尽量避免全局变量
- 所有 GPIO 集中管理（便于画 PCB）
- 新增代码局限在 board 目录，不触碰核心语音流程

## 硬件接线

| 外设 | 接口 | 引脚 |
|------|------|------|
| AHT20 | I2C | SDA→GPIO8, SCL→GPIO9 |
| INMP441 | I2S | SCK→GPIO4, WS→GPIO5, SD→GPIO6 |
| MAX98357A | I2S | BCLK→GPIO15, LRC→GPIO16, DIN→GPIO17 |
| RGB LED | 板载 | GPIO48 |
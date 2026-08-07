# Eliya's Mind - 项目进度记录

## 最后更新
2026-08-07 21:58 (Asia/Shanghai)

## 当前状态
小智 ESP-IDF 固件已成功编译烧录，语音对话正常。下一步：实现 AHT20 温湿度语音查询（MCP Tool）。

## 已完成的工作

### 1. Audio 模块验证成功（Arduino 版）
- 桌面 `Eliya's Mind/firmware/` 工程
- INMP441 麦克风 + MAX98357A 功放
- I2S 驱动验证通过
- 串口输出正常（USBSerial）

### 2. 策略变更：改用小智官方 ESP-IDF 源码
- 克隆 `78/xiaozhi-esp32` 到 `reference/xiaozhi-esp32`
- 修改 `bread-compact-wifi/config.h` 适配硬件：
  - MIC: WS=GPIO5, SCK=GPIO4, DIN=GPIO6（互换 WS/SCK）
  - SPK: DOUT=GPIO17, BCLK=GPIO15, LRCK=GPIO16（DOUT 7→17）
- 编译流程：`python scripts/build.py --board bread-compact-wifi`
- 唤醒词：`CONFIG_SR_WN_WN9_NIHAOXIAOZHI_TTS=y`（"你好小智"）

### 3. ESP-IDF 安装完成
- ✅ ESP-IDF v5.5.5 安装完成
- ✅ xiaozhi 编译烧录成功
- ✅ 音频正常（麦克风 + 扬声器）

### 4. GitHub 项目完善
- ✅ 提交 xiaozhi-esp32 源码到 `firmware/`（含完整板级配置）
- ✅ 更新根 README.md 反映真实进度
- ✅ 更新 docs/Architecture.md（MCP 架构说明）
- ✅ 推送 PROGRESS.md 到远程仓库

## 硬件接线（已验证）
| 外设 | 引脚 | 状态 |
|------|------|------|
| INMP441 SCK | GPIO4 | ✅ |
| INMP441 WS | GPIO5 | ✅ |
| INMP441 SD | GPIO6 | ✅ |
| MAX98357A BCLK | GPIO15 | ✅ |
| MAX98357A LRC | GPIO16 | ✅ |
| MAX98357A DIN | GPIO17 | ✅ |
| AHT20 SDA | GPIO8 | ✅ |
| AHT20 SCL | GPIO9 | ✅ |
| RGB LED | GPIO48 | ✅ |

## 下一步：语音查询温湿度（AHT20 MCP Tool）

### 设计方案
利用小智内置 MCP（Model Context Protocol）框架，注册自定义工具：

1. **新建** `aht20_sensor.h` / `aht20_sensor.cc`（board 目录下）
   - 移植自 `esp32_test/firmware/main/sensors/aht20/`（已验证）
   - 使用 ESP-IDF `driver/i2c_master.h` API
   - I2C: SDA=GPIO8, SCL=GPIO9

2. **修改** `compact_wifi_board.cc` 的 `InitializeTools()`
   - 新建 I2C 总线（GPIO8/9，I2C_NUM_1）
   - 初始化 AHT20
   - 注册 MCP 工具：`self.sensor.get_temperature_humidity`

3. **修改** `config.h`
   - 添加 `AHT20_SDA_PIN GPIO_NUM_8`、`AHT20_SCL_PIN GPIO_NUM_9`

### 关键路径
- ESP-IDF: `C:\esp-idf`
- 小智项目: `c:\Users\13455\Desktop\Eliya's mind\reference\xiaozhi-esp32`
- 板级配置: `reference\xiaozhi-esp32\main\boards\bread-compact-wifi\`
- 参考驱动: `C:\Users\13455\Documents\PlatformIO\Projects\esp32_test\firmware\main\sensors\aht20\`

## 待完成任务
- [ ] 编写 AHT20 驱动（aht20_sensor.h/cc）
- [ ] 修改 compact_wifi_board.cc 注册 MCP 工具
- [ ] 修改 config.h 添加 AHT20 引脚
- [ ] 编译烧录验证
- [ ] 语音测试（"你好小智，现在温度多少？"）

## 备注
- 用户明确要求：不要自己实现 WebSocket/MQTT/Opus/ESP-SR，全部用小智官方源码
- 只修改 config.h 和 board 目录，不破坏官方项目结构
- 唤醒词是"你好小智"（CONFIG_SR_WN_WN9_NIHAOXIAOZHI_TTS）
- MCP 自定义工具必须在板级 `InitializeTools()` 中注册（mcp_server.cc 官方注释明确）
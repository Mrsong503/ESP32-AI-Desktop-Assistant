# Eliya's Mind - 项目进度记录

## 最后更新
2026-08-05 02:45 (Asia/Shanghai)

## 当前状态
用户去睡觉了，明天继续。ESP-IDF 工具链下载超时，需要继续。

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

### 3. ESP-IDF 安装进度
- ✅ git clone 完成（140 个组件 + 所有子模块）
- ✅ Python 3.11 安装成功（`C:\Users\13455\AppData\Local\Programs\Python\Python311`）
- ⏳ install.bat 下载工具链超时（10分钟限制，约 21%）
- PlatformIO Core 6.1.19 已安装

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

## 下一步（明天继续）

### 选项 A：继续下载 ESP-IDF 工具链
```powershell
# 1. 设置 Python 3.11
$env:PATH = 'C:\Users\13455\AppData\Local\Programs\Python\Python311;C:\Users\13455\AppData\Local\Programs\Python\Python311\Scripts;' + $env:PATH

# 2. 进入 ESP-IDF 目录
cd C:\esp-idf

# 3. 运行 install.bat（增加超时时间）
.\install.bat esp32s3
```

### 选项 B：检查 PlatformIO 是否已有 ESP-IDF
```powershell
# PlatformIO 可能已经安装了 ESP-IDF 工具链
# 检查版本
& 'C:\Users\13455\.platformio\penv\Scripts\pio.exe' --version
```

### 选项 C：直接用 PlatformIO 编译小智项目
如果 PlatformIO 已有 ESP-IDF，可以直接编译：
```powershell
# 进入小智项目
cd "C:\Users\13455\Desktop\Eliya's mind\reference\xiaozhi-esp32"

# 用 Python 3.11 编译
python scripts/build.py --board bread-compact-wifi
```

## 关键路径
- ESP-IDF: `C:\esp-idf`
- 小智项目: `C:\Users\13455\Desktop\Eliya's mind\reference\xiaozhi-esp32`
- 配置文件: `reference\xiaozhi-esp32\main\boards\bread-compact-wifi\config.h`
- PlatformIO: `C:\Users\13455\.platformio\`

## 待完成任务
- [ ] ESP-IDF 工具链安装完成
- [ ] 编译小智固件（`python scripts/build.py --board bread-compact-wifi`）
- [ ] 烧录到 ESP32-S3
- [ ] 配网（连接 WiFi）
- [ ] 唤醒词测试（"小智小智"）

## 备注
- 用户明确要求：不要自己实现 WebSocket/MQTT/Opus/ESP-SR，全部用小智官方源码
- 只修改 config.h 适配硬件，不破坏官方项目结构
- 唤醒词是"你好小智"（CONFIG_SR_WN_WN9_NIHAOXIAOZHI_TTS）
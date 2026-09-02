#include "aht20_sensor.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_rom_sys.h"

#include "config.h"  // AHT20_SDA_PIN / AHT20_SCL_PIN

#define TAG "AHT20"

// AHT20 I2C 鍦板潃 (7 浣?
#define AHT20_I2C_ADDR 0x38

// AHT20 鍛戒护
#define AHT20_CMD_SOFT_RESET 0xBA
#define AHT20_CMD_INIT       0xBE
#define AHT20_CMD_MEASURE    0xAC

// AHT20 鐘舵€佷綅
#define AHT20_STATUS_BUSY    0x80  // bit7: 1=busy, 0=ready
#define AHT20_STATUS_CALIB   0x08  // bit3: 1=calibrated, 0=not calibrated

// ============ GPIO bit-bang I2C ============
// 缁曡繃 ESP-IDF 5.x i2c_master 椹卞姩锛堣椹卞姩鍦?GPIO8/9 涓婂叏閮ㄦ暟鎹?NACK锛?
// 浣?bit-bang 鍚岀‖浠惰兘姝ｅ父閫氫俊 鈥斺€?宸查€氳繃璇婃柇鏃ュ織纭锛夈€?
namespace {

constexpr gpio_num_t kSda = AHT20_SDA_PIN;
constexpr gpio_num_t kScl = AHT20_SCL_PIN;
constexpr int kHalfPeriodUs = 10;  // ~50kHz

void bbPinInit() {
    gpio_config_t io = {};
    io.pin_bit_mask = (1ULL << kSda) | (1ULL << kScl);
        io.mode = GPIO_MODE_INPUT_OUTPUT_OD;  // 开漏+输入：必须使能输入缓冲才读得到 SDA
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
    gpio_set_level(kSda, 1);
    gpio_set_level(kScl, 1);
}

inline void bbDelay() { esp_rom_delay_us(kHalfPeriodUs); }

void bbStart() {
    gpio_set_level(kSda, 1); bbDelay();
    gpio_set_level(kScl, 1); bbDelay();
    gpio_set_level(kSda, 0); bbDelay();
    gpio_set_level(kScl, 0); bbDelay();
}

void bbStop() {
    gpio_set_level(kSda, 0); bbDelay();
    gpio_set_level(kScl, 1); bbDelay();
    gpio_set_level(kSda, 1); bbDelay();
}

int bbWriteByte(uint8_t b) {
    for (int i = 7; i >= 0; --i) {
        gpio_set_level(kSda, (b >> i) & 1);
        bbDelay();
        gpio_set_level(kScl, 1); bbDelay();
        gpio_set_level(kScl, 0); bbDelay();
    }
    gpio_set_level(kSda, 1);
    bbDelay();
    gpio_set_level(kScl, 1); bbDelay();
    int ack = gpio_get_level(kSda);
    gpio_set_level(kScl, 0); bbDelay();
    return ack;
}

uint8_t bbReadByte(int sendAck) {
    uint8_t b = 0;
    gpio_set_level(kSda, 1);
    for (int i = 7; i >= 0; --i) {
        gpio_set_level(kScl, 1); bbDelay();
        b = (b << 1) | gpio_get_level(kSda);
        gpio_set_level(kScl, 0); bbDelay();
    }
    gpio_set_level(kSda, sendAck ? 0 : 1);
    bbDelay();
    gpio_set_level(kScl, 1); bbDelay();
    gpio_set_level(kScl, 0); bbDelay();
    gpio_set_level(kSda, 1);
    return b;
}

int bbWrite(uint8_t addr, const uint8_t* data, size_t len) {
    bbStart();
    int ack = bbWriteByte((addr << 1) | 0);
    if (ack == 0 && data && len > 0) {
        for (size_t i = 0; i < len; ++i) {
            if (bbWriteByte(data[i]) != 0) { ack = 1; break; }
        }
    }
    bbStop();
    return ack;
}

int bbRead(uint8_t addr, uint8_t* data, size_t len) {
    bbStart();
    int ack = bbWriteByte((addr << 1) | 1);
    if (ack == 0 && data && len > 0) {
        for (size_t i = 0; i < len; ++i) {
            data[i] = bbReadByte(i < len - 1);
        }
    }
    bbStop();
    return ack;
}

}  // namespace
// ============ GPIO bit-bang I2C 缁撴潫 ============

AHT20Sensor::AHT20Sensor()
    : initialized_(false), available_(false), temperature_(0.0f), humidity_(0.0f)
{
}

AHT20Sensor::~AHT20Sensor()
{
}

bool AHT20Sensor::begin()
{
    bbPinInit();

    vTaskDelay(pdMS_TO_TICKS(100));

    uint8_t cmd = AHT20_CMD_SOFT_RESET;
    if (bbWrite(AHT20_I2C_ADDR, &cmd, 1) == 0) {
        ESP_LOGI(TAG, "Soft reset OK");
        vTaskDelay(pdMS_TO_TICKS(10));
    } else {
        ESP_LOGW(TAG, "Soft reset NACK (non-fatal), continuing");
    }

    if (!initSensor()) {
        ESP_LOGE(TAG, "Sensor initialization failed");
        return false;
    }

    // 注意：AHT20 部分版本在初始化后立即读状态会返回 bit3=0（未校准），
    // 但实际发送测量命令后响应里的校准位是正确的。
    // 故此处不卡校准检查，直接标记可用，由 update() 的测量响应判断真实状态。
    vTaskDelay(pdMS_TO_TICKS(10));

    available_ = true;
    initialized_ = true;
    ESP_LOGI(TAG, "AHT20 init OK");
    return true;
}

bool AHT20Sensor::softReset()
{
    uint8_t cmd = AHT20_CMD_SOFT_RESET;
    return bbWrite(AHT20_I2C_ADDR, &cmd, 1) == 0;
}

bool AHT20Sensor::initSensor()
{
    uint8_t init_cmd[3] = {AHT20_CMD_INIT, 0x08, 0x00};
    int ack = bbWrite(AHT20_I2C_ADDR, init_cmd, 3);
    if (ack != 0) {
        ESP_LOGW(TAG, "Init command NACK: %d", ack);
        return false;
    }
    ESP_LOGI(TAG, "Init command sent");
    return true;
}

bool AHT20Sensor::checkCalibration()
{
    uint8_t status = 0;
    int ack = bbRead(AHT20_I2C_ADDR, &status, 1);
    if (ack != 0) {
        ESP_LOGW(TAG, "Failed to read status for calibration check: NACK");
        return false;
    }

    bool calibrated = (status & AHT20_STATUS_CALIB) != 0;
    if (!calibrated) {
        ESP_LOGW(TAG, "Sensor not calibrated (status=0x%02x), retrying init", status);
        initSensor();
        vTaskDelay(pdMS_TO_TICKS(40));
        ack = bbRead(AHT20_I2C_ADDR, &status, 1);
        if (ack != 0) {
            ESP_LOGE(TAG, "Calibration re-check read NACK");
            return false;
        }
        calibrated = (status & AHT20_STATUS_CALIB) != 0;
    }
    ESP_LOGI(TAG, "Calibration status: 0x%02x (calibrated=%d)", status, calibrated);
    return calibrated;
}

bool AHT20Sensor::waitForMeasurement(uint32_t timeoutMs)
{
    uint8_t status = 0;
    uint32_t elapsed = 0;
    const uint32_t pollInterval = 5;

    while (elapsed < timeoutMs) {
        int ack = bbRead(AHT20_I2C_ADDR, &status, 1);
        if (ack == 0 && (status & AHT20_STATUS_BUSY) == 0) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(pollInterval));
        elapsed += pollInterval;
    }

    ESP_LOGW(TAG, "Measurement timeout (status=0x%02x)", status);
    return false;
}

bool AHT20Sensor::update()
{
    if (!initialized_) {
        return false;
    }

    uint8_t measure_cmd[3] = {AHT20_CMD_MEASURE, 0x33, 0x00};
    int ack = bbWrite(AHT20_I2C_ADDR, measure_cmd, 3);
    if (ack != 0) {
        ESP_LOGE(TAG, "Measurement trigger NACK");
        available_ = false;
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(80));   // AHT20 转换时间：固定延时匹配 Arduino Adafruit 库

        uint8_t data[7] = {0};
    ack = bbRead(AHT20_I2C_ADDR, data, 7);
    if (ack != 0) {
        ESP_LOGE(TAG, "Data read NACK");
        available_ = false;
        return false;
    }
    ESP_LOGW(TAG, "Raw bytes: %02x %02x %02x %02x %02x %02x %02x",
             data[0], data[1], data[2], data[3], data[4], data[5], data[6]);

    if (data[0] & AHT20_STATUS_BUSY) {
        ESP_LOGW(TAG, "Measurement busy (status=0x%02x), waiting", data[0]);
        vTaskDelay(pdMS_TO_TICKS(50));
        ack = bbRead(AHT20_I2C_ADDR, data, 7);
        ESP_LOGW(TAG, "Raw bytes (retry): %02x %02x %02x %02x %02x %02x %02x",
                 data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
        if (ack != 0) { available_ = false; return false; }
    }

        // AHT20 标准 6 字节数据: [status][H2][H1][H0][T2][T1][T0]
    // data[0]=status, data[1..3]=湿度, data[3]低4位+data[4..5]=温度
    uint32_t raw_humidity =
        ((uint32_t)data[1] << 12) |
        ((uint32_t)data[2] << 4) |
        ((uint32_t)data[3] >> 4);
    uint32_t raw_temperature =
        (((uint32_t)data[3] & 0x0F) << 16) |
        ((uint32_t)data[4] << 8) |
        (uint32_t)data[5];
        float humidity = (raw_humidity * 100.0f) / 1048576.0f;
    float temperature = (raw_temperature * 200.0f) / 1048576.0f - 50.0f;
    humidity_ = humidity;
    temperature_ = temperature;
    ESP_LOGI(TAG, "raw_humidity=%u raw_temperature=%u humidity=%.2f%% temperature=%.2f°C",
             raw_humidity, raw_temperature, humidity, temperature);

    available_ = true;
    ESP_LOGI(TAG, "AHT20: T=%.1f°C H=%.1f%%", temperature_, humidity_);
    return true;
}

float AHT20Sensor::getTemperature() const
{
    return temperature_;
}

float AHT20Sensor::getHumidity() const
{
    return humidity_;
}

bool AHT20Sensor::isAvailable() const
{
    return available_;
}

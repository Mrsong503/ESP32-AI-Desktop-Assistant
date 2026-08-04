#include "mic.h"

#include <Arduino.h>
#include <driver/i2s.h>

#include "../../config/pins.h"

// INMP441 -> 32-bit 帧，左声道
#define I2S_MIC_PORT I2S_NUM_0

static const i2s_config_t kMicConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};

static const i2s_pin_config_t kMicPins = {
    .bck_io_num = PIN_MIC_SCK,
    .ws_io_num = PIN_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PIN_MIC_SD,
};

bool Mic::begin()
{
    if (initialized_)
        return true;

    esp_err_t err = i2s_driver_install(I2S_MIC_PORT, &kMicConfig, 0, NULL);
    if (err != ESP_OK)
        return false;

    err = i2s_set_pin(I2S_MIC_PORT, &kMicPins);
    if (err != ESP_OK)
    {
        i2s_driver_uninstall(I2S_MIC_PORT);
        return false;
    }

    initialized_ = true;
    return true;
}

void Mic::end()
{
    if (!initialized_)
        return;
    i2s_driver_uninstall(I2S_MIC_PORT);
    initialized_ = false;
}

size_t Mic::read(int32_t* buffer, size_t samples)
{
    if (!initialized_ || !buffer)
        return 0;

    size_t bytesRead = 0;
    esp_err_t err = i2s_read(I2S_MIC_PORT, buffer, samples * sizeof(int32_t), &bytesRead, portMAX_DELAY);
    if (err != ESP_OK)
        return 0;

    return bytesRead / sizeof(int32_t);
}

float Mic::computeLevel(const int32_t* buffer, size_t samples)
{
    if (samples == 0)
        return 0.0f;

    int64_t sum = 0;
    for (size_t i = 0; i < samples; i++)
    {
        int32_t v = buffer[i] >> 8; // 32bit -> 24bit
        if (v < 0)
            v = -v;
        sum += v;
    }

    float avg = (float)sum / samples;
    float norm = avg / 8388608.0f; // 2^23
    return constrain(norm, 0.0f, 1.0f);
}

float Mic::getLevel()
{
    if (!initialized_)
        return 0.0f;

    int32_t buffer[512];
    size_t got = read(buffer, 512);
    return computeLevel(buffer, got);
}

int Mic::getPeak()
{
    if (!initialized_)
        return 0;

    int32_t buffer[512];
    size_t got = read(buffer, 512);

    int32_t peak = 0;
    for (size_t i = 0; i < got; i++)
    {
        int32_t v = buffer[i] >> 8;
        if (v < 0)
            v = -v;
        if (v > peak)
            peak = v;
    }
    return (int)(((float)peak / 8388608.0f) * 1000.0f);
}
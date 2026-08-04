#include "speaker.h"

#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#include "../../config/pins.h"

// MAX98357A -> 16-bit I2S 标准格式，单声道
#define I2S_SPK_PORT I2S_NUM_1

static const uint32_t SAMPLE_RATE = 24000;

static const i2s_config_t kSpkConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
};

static const i2s_pin_config_t kSpkPins = {
    .bck_io_num = PIN_SPK_BCLK,
    .ws_io_num = PIN_SPK_LRC,
    .data_out_num = PIN_SPK_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE,
};

bool Speaker::begin()
{
    if (initialized_)
        return true;

    esp_err_t err = i2s_driver_install(I2S_SPK_PORT, &kSpkConfig, 0, NULL);
    if (err != ESP_OK)
        return false;

    err = i2s_set_pin(I2S_SPK_PORT, &kSpkPins);
    if (err != ESP_OK)
    {
        i2s_driver_uninstall(I2S_SPK_PORT);
        return false;
    }

    initialized_ = true;
    return true;
}

void Speaker::end()
{
    if (!initialized_)
        return;
    i2s_driver_uninstall(I2S_SPK_PORT);
    initialized_ = false;
}

size_t Speaker::write(const int16_t* data, size_t samples)
{
    if (!initialized_ || !data)
        return 0;

    size_t bytesWritten = 0;
    esp_err_t err = i2s_write(I2S_SPK_PORT, data, samples * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    if (err != ESP_OK)
        return 0;

    return bytesWritten / sizeof(int16_t);
}

void Speaker::playTestTone(float frequency, uint32_t durationMs)
{
    if (!initialized_)
        return;

    const size_t BUF_SIZE = 480; // 20ms @24kHz
    int16_t buffer[BUF_SIZE];

    uint32_t samplesWritten = 0;
    uint32_t totalSamples = (SAMPLE_RATE * durationMs) / 1000;

    float phase = 0.0f;
    const float phaseStep = 2.0f * PI * frequency / SAMPLE_RATE;
    const int16_t amplitude = 12000; // 50% 音量（MAX98357A GAIN 悬空约 9dB）

    while (samplesWritten < totalSamples)
    {
        size_t count = BUF_SIZE;
        if (samplesWritten + count > totalSamples)
            count = totalSamples - samplesWritten;

        for (size_t i = 0; i < count; i++)
        {
            buffer[i] = (int16_t)(amplitude * sinf(phase));
            phase += phaseStep;
            if (phase >= 2.0f * PI)
                phase -= 2.0f * PI;
        }

        write(buffer, count);
        samplesWritten += count;
    }
}
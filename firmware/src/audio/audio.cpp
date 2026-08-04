#include "audio.h"

#include <Arduino.h>
#include <HWCDC.h>

bool Audio::begin()
{
    bool ok = true;

    if (!mic_.begin())
    {
        USBSerial.println("[Audio] MIC (INMP441) init failed!");
        ok = false;
    }
    else
    {
        USBSerial.println("[Audio] MIC (INMP441) OK");
    }

    if (!speaker_.begin())
    {
        USBSerial.println("[Audio] Speaker (MAX98357A) init failed!");
        ok = false;
    }
    else
    {
        USBSerial.println("[Audio] Speaker (MAX98357A) OK");
    }

    return ok;
}

void Audio::end()
{
    mic_.end();
    speaker_.end();
}

Mic& Audio::mic()
{
    return mic_;
}

Speaker& Audio::speaker()
{
    return speaker_;
}

void Audio::playTestTone(float frequency, uint32_t durationMs)
{
    USBSerial.printf("[Audio] Playing test tone: %.0f Hz, %u ms\n", frequency, durationMs);
    speaker_.playTestTone(frequency, durationMs);
}

void Audio::recordTest(uint32_t durationMs)
{
    const size_t SAMPLE_BUF = 512;
    int32_t buffer[SAMPLE_BUF];

    uint32_t start = millis();
    uint32_t lastPrint = 0;

    USBSerial.printf("[Audio] Recording for %u ms...\n", durationMs);

    while (millis() - start < durationMs)
    {
        size_t got = mic_.read(buffer, SAMPLE_BUF);

        if (got > 0)
        {
            // 每 200ms 打印一次当前音量
            if (millis() - lastPrint >= 200)
            {
                float level = mic_.getLevel();
                int peak = mic_.getPeak();

                int bars = (int)(level * 40);
                USBSerial.printf("[MIC] level=%5.2f peak=%4d |", level, peak);
                for (int i = 0; i < 40; i++)
                    USBSerial.print(i < bars ? "█" : "-");
                USBSerial.println();

                lastPrint = millis();
            }
        }
        else
        {
            USBSerial.println("[Audio] MIC read timeout!");
            delay(50);
        }
    }

    USBSerial.println("[Audio] Recording done.");
}
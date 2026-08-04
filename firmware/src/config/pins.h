#pragma once

//======================================
// Eliya's Mind - GPIO 引脚集中管理
// 所有引脚在此定义，便于画 PCB
//======================================

// ---------- I2C (AHT20) ----------
#define PIN_I2C_SDA 8
#define PIN_I2C_SCL 9

// ---------- I2S 麦克风 (INMP441) ----------
#define PIN_MIC_SCK 4
#define PIN_MIC_WS  5
#define PIN_MIC_SD  6

// ---------- I2S 功放 (MAX98357A) ----------
#define PIN_SPK_BCLK 15
#define PIN_SPK_LRC  16
#define PIN_SPK_DIN  17

// ---------- RGB LED (NeoPixel) ----------
#define PIN_RGB_LED 48
#define NUM_PIXELS  1

// ---------- 按钮 ----------
#define PIN_BOOT_BUTTON 0
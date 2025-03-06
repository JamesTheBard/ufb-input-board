#ifndef _UFBDISPLAY_HPP
#define _UFBDISPLAY_HPP

#include <Arduino.h>
#include <Wire.h>
#include "inputs.hpp"
#include <U8g2lib.h>
#include <atomic>

#define I2C0_SDA 4
#define I2C0_SCL 5

#define DISP_WIDTH  128
#define DISP_HEIGHT  64

enum class DisplayOptions {
    FIGHTSTICK,
    HITBOX,
    CONTROLLER
};

struct DisplayConfig {
    std::atomic<bool> config_loaded = false;
    std::atomic<uint8_t> address = 0x3C;
    String type = "SSD1306";
    String resolution = "128x64";
};

extern DisplayConfig display_config;

void initDisplay(DisplayConfig &config);
void drawSquare(uint8_t x, uint8_t y, bool enabled);
void drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool enabled);
void drawCircle(uint8_t x, uint8_t y, bool enabled);
bool readInput(uint32_t data, uint8_t input);
void drawOutputs(uint8_t line, uint32_t data, DisplayOptions display_type);
void drawInputs(uint8_t line, uint32_t data);

void drawScreen(uint32_t input_data, uint32_t output_data, Profile &profile_name, uint8_t profile_num);

#endif // _UFBDISPLAY_HPP
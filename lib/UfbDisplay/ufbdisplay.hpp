#ifndef _UFBDISPLAY_HPP
#define _UFBDISPLAY_HPP

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C0_SDA 4
#define I2C0_SCL 5

#define DISP_WIDTH  128
#define DISP_HEIGHT  64

extern Adafruit_SSD1306 display;

enum class DisplayOptions {
    FIGHTSTICK,
    HITBOX,
    CONTROLLER
};

void drawSquare(uint8_t x, uint8_t y, bool enabled);
void drawCircle(uint8_t x, uint8_t y, bool enabled);
bool readInput(uint32_t data, uint8_t input);
void drawOutputs(uint8_t line, uint32_t data, DisplayOptions display_type);
void drawInputs(uint8_t line, uint32_t data);

void drawScreen(uint32_t input_data, uint32_t output_data, String profile_name, uint8_t profile_num, DisplayOptions display_type);
void drawScreen(uint32_t input_data, uint32_t output_data, String profile_name, uint8_t profile_num, uint8_t display_type);

#endif // _UFBDISPLAY_HPP
#include "ufbdisplay.hpp"

Adafruit_SSD1306 display(DISP_WIDTH, DISP_HEIGHT, &Wire, -1);
uint8_t input_width = 8;

/**
 * Draws a square on the attached display that's associated with an 
 * input.
 * 
 * @param x the X position of the top-left corner
 * @param y the Y position of the top-left corner
 * @param enabled whether the associated input is enabled
 */
void drawSquare(uint8_t x, uint8_t y, bool enabled) {
    uint8_t w = 6;
    if (enabled) {
        display.fillRect(x, y, w, w, SSD1306_WHITE);
    } else {
        display.drawRect(x, y, w, w, SSD1306_WHITE);
    }
}

/**
 * Draws a circle on the attached display that is associated with an
 * input.
 * 
 * @param x the X position of the center of the circle
 * @param y the Y position of the center of the circle
 * @param enabled whether the associated input is enabled
 */
void drawCircle(uint8_t x, uint8_t y, bool enabled) {
    uint8_t radius = 4;
    if (enabled) {
        display.fillCircle(x, y, radius, SSD1306_WHITE);
    } else {
        display.drawCircle(x, y, radius, SSD1306_WHITE);
    }
}

/**
 * Given data, determine if a given input is enabled or not.
 * 
 * @param data the input data
 * @param input the input to check
 * @returns the current state of the input
 */
bool readInput(uint32_t data, uint8_t input) {
    if (input == 0) return false;
    return (data >> --input) & 1;
}

/**
 * Draw the outputs to the screen.
 * 
 * @param line the line on the display to start drawing the inputs
 * @param data the output data
 */
void drawOutputs(uint8_t line, uint32_t data) {
    uint8_t sh = 7;
    uint8_t ch = 10;
    uint8_t chh = (int)ch / 2;

    drawSquare(0, line + sh,  readInput(data, 12));       // left
    drawSquare(2 * sh, line + sh, readInput(data, 13));   // right
    drawSquare(sh, line, readInput(data, 15));            // up
    drawSquare(sh, line + (2 * sh), readInput(data, 14)); // down

    drawCircle(3 * ch + 1, line + chh, readInput(data, 8)); // 1P - Square
    drawCircle(4 * ch + 1, line + chh, readInput(data, 7)); // 2P - Triangle
    drawCircle(5 * ch + 1, line + chh, readInput(data, 6)); // 3P - R1
    drawCircle(6 * ch + 1, line + chh, readInput(data, 5)); // 4P - L1

    drawCircle(3 * ch + 1, line + chh + ch, readInput(data, 4)); // 1K - Cross
    drawCircle(4 * ch + 1, line + chh + ch, readInput(data, 3)); // 2K - Circle
    drawCircle(5 * ch + 1, line + chh + ch, readInput(data, 2)); // 3K - R2
    drawCircle(6 * ch + 1, line + chh + ch, readInput(data, 1)); // 4K - L2

    drawSquare(7 * ch + chh, line + 2, readInput(data, 11));             // Select
    drawSquare(7 * ch + chh + sh, line + 2, readInput(data, 9));         // Start
    drawSquare(7 * ch + chh + (2 * sh), line + 2, readInput(data,  10)); // Home

    drawCircle(8 * ch - 1, line + chh + ch, readInput(data, 16)); // L3
    drawCircle(9 * ch, line + chh + ch, readInput(data, 17));     // R3

    if (readInput(data, 18)) {
        display.setCursor(10 * ch + 9, line + 7);
        display.print("TPK");
    }
}

/**
 * Draw the inputs to the screen.
 * 
 * @param line the line to start drawing the inputs on
 * @param data the input data
 */
void drawInputs(int8_t line, uint32_t data) {
    for (int i = 0; i < 16; i ++) {
        if (data >> i & 1) {
            display.fillRect(i * input_width, line, input_width - 1, input_width - 1, SSD1306_WHITE);
        } else {
            display.drawRect(i * input_width, line, input_width - 1, input_width - 1, SSD1306_WHITE);
        }
        if (data >> (i + 16) & 1) {
            display.fillRect(i * input_width, line + input_width, input_width - 1, input_width - 1, SSD1306_WHITE);
        } else {
            display.drawRect(i * input_width, line + input_width, input_width - 1, input_width - 1, SSD1306_WHITE);
        }
    }
}

/**
 * Draw the screen with all of the inputs and outputs.
 * 
 * @param input_data the input data
 * @param output_data the output data
 * @param profile the name of the profile in use
 * @param profile_num the number of the profile in use
 */
void drawScreen(uint32_t input_data, uint32_t output_data, String profile_name, uint8_t profile_num) {
    display.setCursor(0, 6);
    display.clearDisplay();
    display.println(F("Current inputs"));
    if (input_data >> 29 && 1) {
        display.setCursor(96, 6);
        display.print("Unlocked");
    }
    drawInputs(8, input_data);
    display.fillRoundRect(0, 31, 7, 7, 1, SSD1306_WHITE);
    display.setCursor(2, 37);
    display.setTextColor(BLACK);
    display.print(profile_num);
    display.setTextColor(WHITE);
    display.setCursor(9, 37);
    display.println(profile_name);
    drawOutputs(40, output_data);
    display.display();
}

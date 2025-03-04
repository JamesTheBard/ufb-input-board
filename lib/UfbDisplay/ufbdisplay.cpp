#include "ufbdisplay.hpp"

U8G2 display;
uint8_t input_width = 8;

/**
 * Init display
 * 
 * @param display the type of display used
 * 
 */
void initDisplay(String display_type) {
    if (display_type == "SH1106") {
        display = U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE, I2C0_SCL, I2C0_SDA);
    }
    else if (display_type == "SSD1309" || display_type == "CH1119") {
        display = U8G2_SSD1309_128X64_NONAME0_1_HW_I2C(U8G2_R0, U8X8_PIN_NONE, I2C0_SCL, I2C0_SDA);
    }
    else {
        display = U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE, I2C0_SCL, I2C0_SDA);
    }
    display.begin();
    display.setContrast(10);
    display.setFont(u8g2_font_spleen5x8_mr);
}

/**
 * Draws a rectangle on the attached display that's associated with an
 * input.
 * 
 * @param x the X position of the top-left corner
 * @param y the Y position of the top-left corner
 * @param w the width of the rectangle
 * @param h the height of the rectangle
 * @param enabled if the input is enabled
 */
void drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool enabled) {
    display.setDrawColor(1);
    if (enabled) {
        display.drawBox(x, y, w, h);
    } else {
        display.drawFrame(x, y, w, h);
    }
}

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
    drawRectangle(x, y, w, w, enabled);
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
        display.drawDisc(x, y, radius, U8G2_DRAW_ALL);
    } else {
        display.drawCircle(x, y, radius, U8G2_DRAW_ALL);
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
 * Draw the outputs on the display using the generic fightstick layout.
 * 
 * @param line the line of the display to start drawing the outputs
 * @param data the output data
 */
void drawOutputsFightstick(uint8_t line, uint32_t data) {
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
 * Draw the outputs on the display using the generic fightstick layout.
 * 
 * @param line the line of the display to start drawing the outputs
 * @param data the output data
 */
void drawOutputsHitbox(uint8_t line, uint32_t data) {
    uint8_t sh = 7;
    uint8_t ch = 10;
    uint8_t chh = (int)ch / 2;

    drawCircle(chh, line + chh, readInput(data, 12));                 // left
    drawCircle(chh + ch, line + chh, readInput(data, 14));            // down
    drawCircle(2 * ch + 4, line + ch - 1, readInput(data, 13));       // right
    drawCircle(2 * ch + chh, line + 2 * ch - 1, readInput(data, 15)); // up

    drawCircle(3 * ch + 5, line + chh, readInput(data, 8)); // 1P - Square
    drawCircle(4 * ch + 5, line + chh, readInput(data, 7)); // 2P - Triangle
    drawCircle(5 * ch + 5, line + chh, readInput(data, 6)); // 3P - R1
    drawCircle(6 * ch + 5, line + chh, readInput(data, 5)); // 4P - L1

    drawCircle(3 * ch + 5, line + chh + ch, readInput(data, 4)); // 1K - Cross
    drawCircle(4 * ch + 5, line + chh + ch, readInput(data, 3)); // 2K - Circle
    drawCircle(5 * ch + 5, line + chh + ch, readInput(data, 2)); // 3K - R2
    drawCircle(6 * ch + 5, line + chh + ch, readInput(data, 1)); // 4K - L2

    drawSquare(7 * ch + chh + 4, line + 2, readInput(data, 11));             // Select
    drawSquare(7 * ch + chh + sh + 4, line + 2, readInput(data, 9));         // Start
    drawSquare(7 * ch + chh + (2 * sh) + 4, line + 2, readInput(data,  10)); // Home

    drawCircle(8 * ch + 3, line + chh + ch, readInput(data, 16)); // L3
    drawCircle(9 * ch + 4, line + chh + ch, readInput(data, 17)); // R3

    if (readInput(data, 18)) {
        display.setCursor(10 * ch + 9, line + 7);
        display.print("TPK");
    }
}

/**
 * Draw the outputs on the display in the style of a PS/XBOX controller layout.
 * 
 * @param line the line on the display to start drawing the layout
 * @param data the output data
 */
void drawOutputsController(uint8_t line, uint32_t data) {
    uint8_t sh = 7;
    uint8_t ch = 10;
    uint8_t chh = (int)ch / 2;

    drawSquare(0, line + sh,  readInput(data, 12));       // left
    drawSquare(2 * sh, line + sh, readInput(data, 13));   // right
    drawSquare(sh, line, readInput(data, 15));            // up
    drawSquare(sh, line + (2 * sh), readInput(data, 14)); // down

    drawRectangle(3 * sh + 4, line + 2, 6, 4, readInput(data, 11)); // Select
    drawRectangle(4 * sh + 4, line + 2, 6, 4, readInput(data, 9));  // Start
    drawCircle(4 * sh + 3, line + 12, readInput(data, 10));         // Home

    drawSquare(6 * sh + 1, line + sh,  readInput(data, 8));      // square
    drawSquare(8 * sh + 1, line + sh, readInput(data, 3));       // circle
    drawSquare(7 * sh + 1, line, readInput(data, 7));            // triangle
    drawSquare(7 * sh + 1, line + (2 * sh), readInput(data, 4)); // cross

    drawRectangle(10 * sh, line, 10, 6, readInput(data, 5));           // L1
    drawRectangle(10 * sh + 11, line, 10, 6, readInput(data, 6));      // R1
    drawRectangle(10 * sh, line + sh, 10, 6, readInput(data, 1));      // L2
    drawRectangle(10 * sh + 11, line + sh, 10, 6, readInput(data, 2)); // R2
    drawCircle(7 * ch + 5, line + chh + ch + 3, readInput(data, 16));  // L3
    drawCircle(8 * ch + 5, line + chh + ch + 3, readInput(data, 17));  // R3

    if (readInput(data, 18)) {
        display.setCursor(10 * ch + 9, line + 7);
        display.print("TPK");
    }
}

/**
 * Draw the outputs to the screen.
 * 
 * @param line the line on the display to start drawing the inputs
 * @param data the output data
 * @param display_type the layout of the outputs
 */
void drawOutputs(uint8_t line, uint32_t data, DisplayOptions display_type) {
    switch(display_type) {
        case DisplayOptions::FIGHTSTICK:
            drawOutputsFightstick(line, data);
            return;
        case DisplayOptions::CONTROLLER:
            drawOutputsController(line, data);
            return;
        case DisplayOptions::HITBOX:
            drawOutputsHitbox(line, data);
            return;
        default:
            return;
    }
}

/**
 * Draw the inputs to the screen.
 * 
 * @param line the line to start drawing the inputs on
 * @param data the input data
 */
void drawInputs(uint8_t line, uint32_t data) {
    for (int i = 0; i < 16; i ++) {
        drawRectangle(i * input_width, line, input_width - 1, input_width - 1, data >> i & 1);
        drawRectangle(i * input_width, line + input_width, input_width - 1, input_width - 1, data >> (i + 16) & 1);
    }
}

/**
 * Draw the screen with all of the inputs and outputs.
 * 
 * @param input_data the input data
 * @param output_data the output data
 * @param profile the name of the profile in use
 * @param profile_num the number of the profile in use
 * @param display_type the display layout to use
 */
void drawScreen(uint32_t input_data, uint32_t output_data, String profile_name, uint8_t profile_num, DisplayOptions display_type) {
    // Draw upper-half (inputs)
    display.setFont(u8g2_font_spleen5x8_mr);
    display.setCursor(0, 6);
    display.println("Current inputs");
    if (readInput(input_data, 30)) {
        display.setFont(u8g2_font_tom_thumb_4x6_tr);
        display.drawRBox(100, 0, 27, 7, 1);
        display.setDrawColor(0);
        display.setCursor(102, 6);
        display.print("Unlock");
        display.setDrawColor(1);
        display.setFont(u8g2_font_spleen5x8_mr);
    }
    drawInputs(8, input_data);

    // Draw lower-half (outputs)
    display.drawRBox(0, 30, 9, 8, 1);
    display.setFontMode(1);
    display.setFont(u8g2_font_squeezed_b6_tn);
    display.setDrawColor(0);
    display.setCursor(3, 37);
    display.print(profile_num);
    display.setDrawColor(1);
    display.setCursor(12, 37);
    display.setFont(u8g2_font_spleen5x8_mr);
    display.println(profile_name);
    drawOutputs(40, output_data, display_type);
    display.display();
}

/**
 * Draw the screen with all of the inputs and outputs.
 * 
 * @param input_data the input data
 * @param output_data the output data
 * @param profile the name of the profile in use
 * @param profile_num the number of the profile in use
 * @param display_type the display layout to use
 */
void drawScreen(uint32_t input_data, uint32_t output_data, String profile_name, uint8_t profile_num, uint8_t display_type) {
    drawScreen(input_data, output_data, profile_name, profile_num, static_cast<DisplayOptions>(display_type));
}
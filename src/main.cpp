#include <Arduino.h>
#include <SPI.h>
#include <Fonts/TomThumb.h>
#include <ufbdisplay.hpp>
#include <inputs.hpp>

#define UFB_ENABLE 22
#define BOOT_LED 25

uint32_t input_buffer, output_buffer;
uint32_t profile_debounce = 0;

void setup() {
    pinMode(UFB_ENABLE, OUTPUT);
    pinMode(BOOT_LED, OUTPUT);
    digitalWrite(UFB_ENABLE, LOW);

    profiles[1] = {"Passthrough (1:1)"}; // No buttons get remapped

    current_profile.store(1);
    input_data.store(0);
    output_data.store(0);

    Serial.begin(9600);

    loadProfilesFromSDCard(profiles);

    Serial.println("Starting SPI busses...");

    SPI.setRX(SPI0_MISO);
    SPI.setTX(SPI0_MOSI);
    SPI.setSCK(SPI0_SCLK);
    SPI.begin();

    
    pinMode(INPUT_LATCH, OUTPUT);
    pinMode(INPUT_CE, OUTPUT);

    pinMode(OUTPUT_CE, OUTPUT);
    pinMode(OUTPUT_SS, OUTPUT);
    pinMode(OUTPUT_CLR, OUTPUT);

    digitalWrite(INPUT_LATCH, HIGH);
    digitalWrite(INPUT_CE, HIGH);
    digitalWrite(OUTPUT_CE, HIGH);
    digitalWrite(OUTPUT_CLR, HIGH);

    current_profile.store(1);

    Serial.println("Starting controller...");

    SPI.beginTransaction(inputSettings);
    digitalWrite(INPUT_CE, LOW);
    digitalWrite(INPUT_LATCH, LOW);
    digitalWrite(INPUT_LATCH, HIGH);

    SPI.transfer(&input_buffer, 4);

    digitalWrite(INPUT_CE, HIGH);
    SPI.endTransaction();

    input_data.store(input_buffer);
    output_data.store(profiles[current_profile.load()].processInputs(input_buffer));
    output_buffer = reverseBytes(output_data.load()) >> 8;

    // Write all outputs (3 bytes)
    digitalWrite(OUTPUT_CE, LOW);
    digitalWrite(OUTPUT_SS, LOW);

    SPI.beginTransaction(outputSettings);
    SPI.transfer(&output_buffer, 3);
    SPI.endTransaction();
    
    digitalWrite(OUTPUT_SS, HIGH);
    digitalWrite(UFB_ENABLE, HIGH);
    digitalWrite(BOOT_LED, HIGH);
}

void loop(){
    // Read all inputs (4 bytes)
    SPI.beginTransaction(inputSettings);
    digitalWrite(INPUT_CE, LOW);
    digitalWrite(INPUT_LATCH, LOW);
    digitalWrite(INPUT_LATCH, HIGH);

    SPI.transfer(&input_buffer, 4);

    digitalWrite(INPUT_CE, HIGH);
    SPI.endTransaction();

    // Short circuit processing if the inputs haven't changed
    if (input_buffer == input_data.load()) return;

    // Switch profiles based on 31/32
    if (input_buffer & (1 << 29) && millis() > profile_debounce) {
        if (input_buffer & (1 << 30)) {
            uint8_t prev_profile = current_profile.load() - 1;
            if (prev_profile && profiles.count(prev_profile)) {
                current_profile.store(prev_profile);
            }
        }

        if (input_buffer & (1 << 31)) {
            uint8_t next_profile = current_profile.load() + 1;
            if (profiles.count(next_profile)) {
                current_profile.store(next_profile);
            }
        }
        
        if (input_buffer & (3 << 30)) profile_debounce = millis() + 200;
    }

    // Store and process input data
    input_data.store(input_buffer);
    output_data.store(profiles[current_profile.load()].processInputs(input_buffer));
    output_buffer = reverseBytes(output_data.load()) >> 8;

    // Write all outputs (3 bytes)
    digitalWrite(OUTPUT_CE, LOW);
    digitalWrite(OUTPUT_SS, LOW);

    SPI.beginTransaction(outputSettings);
    SPI.transfer(&output_buffer, 3);
    SPI.endTransaction();
    
    digitalWrite(OUTPUT_SS, HIGH);
    // digitalWrite(OUTPUT_CE, HIGH);
}

void setup1() {
    Wire.setSDA(I2C0_SDA);
    Wire.setSCL(I2C0_SCL);
    Wire.setClock(400000);
    display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR);
    display.setFont(&TomThumb);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    delay(2000);
}

void loop1() {
    drawScreen(input_data.load(), output_data.load(), profiles[current_profile.load()].profile_name, current_profile.load());
}
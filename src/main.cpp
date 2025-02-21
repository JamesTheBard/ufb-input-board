#include <Arduino.h>
#include <SPI.h>
#include <Fonts/TomThumb.h>
#include <ufbdisplay.hpp>
#include <inputs.hpp>
#include <atomic>
#include <map>

#define UFB_POWER 25

Inputs *inputs;
std::map<uint8_t, Profile> profiles;

uint32_t input_buffer, output_buffer;
std::atomic<uint32_t> input_data, output_data;
std::atomic<uint8_t> current_profile;
uint8_t profile_state;

void setup() {
    Serial.begin(9600);

    pinMode(UFB_POWER, OUTPUT);

    // Get the data variables initialized
    input_data.store(0);
    output_data.store(0);
    input_buffer = 0;
    output_buffer = 0;

    // Load the profiles from the SD card, set the default profile to
    // passthrough mode.
    Serial.println("Loading profiles from the SD card...");
    profiles[1] = {"Passthrough (1:1)"}; // No buttons get remapped
    current_profile.store(1);
    loadProfilesFromSDCard(profiles);

    // Start reading inputs.  This needs to happen prior to the UFB coming online so
    // that we can keep the mode selection capabilities of the UFB on boot.
    digitalWrite(UFB_POWER, LOW);
    Serial.println("Starting Brook UFB controller...");

    inputs = new Inputs();
    inputs->readInputs(&input_buffer);
    input_data.store(input_buffer);
    output_data.store(profiles[current_profile.load()].processInputs(input_buffer));
    output_buffer = reverseBytes(output_data.load()) >> 8;
    inputs->writeOutputs(&output_buffer);

    digitalWrite(UFB_POWER, HIGH);
}

void loop() {
    input_data.store(input_buffer);
    inputs->readInputs(&input_buffer);

    // Short circuit processing if the inputs haven't changed
    if (input_buffer == input_data.load()) return;

    // Only process profile inputs if Profile Enable (input 30) input
    // is actually enabled.
    profile_state = input_buffer >> 29;
    if (profile_state & 0b001) {
        if (profile_state & 0b011) {
            uint8_t prev_profile = current_profile.load() - 1;
            if (prev_profile < 1) return;
            if (profiles.count(prev_profile) > 0) {
                current_profile.store(prev_profile);
            }
        }

        if (profile_state & 0b101) {
            uint8_t next_profile = current_profile.load() + 1;
            if (profiles.count(next_profile) > 0) {
                current_profile.store(next_profile);
            }
        }
    }

    // Store and process input data
    output_data.store(profiles[current_profile.load()].processInputs(input_buffer));
    output_buffer = reverseBytes(output_data.load()) >> 8;

    // Write all outputs (3 bytes)
    inputs->writeOutputs(&output_buffer);
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
    drawScreen(input_data.load(), output_data.load(), profiles[current_profile.load()].profile_name, current_profile);
}
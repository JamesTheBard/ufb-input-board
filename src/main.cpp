#include <Arduino.h>
#include <SPI.h>
#include <ufbdisplay.hpp>
#include <inputs.hpp>
#include <config.hpp>

#define UFB_ENABLE 22
#define BOOT_LED 25

uint32_t input_buffer, output_buffer;
uint32_t profile_debounce = 0;

std::map<uint8_t, Profile> profiles;
std::atomic<uint32_t> input_data, output_data;
std::atomic<uint8_t> current_profile;


void setup() {
    pinMode(UFB_ENABLE, OUTPUT);
    pinMode(BOOT_LED, OUTPUT);
    digitalWrite(UFB_ENABLE, LOW);

    profiles[1] = {"Passthrough (1:1)"}; // No buttons get remapped

    current_profile.store(1);

    input_data.store(0);
    output_data.store(0);

    Serial.begin(9600);

    Serial.println("Loading config file...");
    display_config.config_loaded.store(loadProfilesFromSDCard(profiles, display_config));

    Serial.println("Starting SPI busses...");

    // Configure the SPI0 bus for reading/writing data
    SPI.setRX(SPI0_MISO);
    SPI.setTX(SPI0_MOSI);
    SPI.setSCK(SPI0_SCLK);
    SPI.begin();

    // Pin configurations
    pinMode(INPUT_LATCH, OUTPUT);
    pinMode(INPUT_CE, OUTPUT);

    pinMode(OUTPUT_CE, OUTPUT);
    pinMode(OUTPUT_SS, OUTPUT);
    pinMode(OUTPUT_CLR, OUTPUT);

    digitalWrite(INPUT_LATCH, HIGH);
    digitalWrite(INPUT_CE, HIGH);
    digitalWrite(OUTPUT_CE, HIGH);
    digitalWrite(OUTPUT_CLR, HIGH);

    Serial.println("Starting controller...");

    // Do an initial read of the inputs...
    SPI.beginTransaction(inputSettings);
    digitalWrite(INPUT_CE, LOW);
    digitalWrite(INPUT_LATCH, LOW);
    digitalWrite(INPUT_LATCH, HIGH);

    SPI.transfer(&input_buffer, 4);

    digitalWrite(INPUT_CE, HIGH);
    SPI.endTransaction();

    // Process the inputs
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

    // Enable the power rail on the UFB.  Need to delay this after the
    // outputs have been set on the adapter board.
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
    uint8_t selected_profile = current_profile.load(); 
    if (input_buffer & (1 << 29) && millis() > profile_debounce) {
        if (input_buffer & (1 << 30)) {
            if (profiles.count(selected_profile - 1)) {
                current_profile--;
                profile_debounce = millis() + 200;
            }
        } else if (input_buffer & (1 << 31)) {
            if (profiles.count(selected_profile + 1)) {
                current_profile++;
                profile_debounce = millis() + 200;
            }
        }
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
}

void setup1() {
    do { delay(10); } while (!display_config.config_loaded.load());
    initDisplay(display_config);
}

uint32_t display_data = 0xFFFFFFFF;

void loop1() {
    // if (display_data == input_data.load()) return;
    // display_data = input_data.load();

    Profile &cprofile = profiles[current_profile.load()];
    drawScreen(input_data.load(), output_data.load(), cprofile, current_profile.load());
}

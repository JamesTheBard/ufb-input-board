#include <Arduino.h>
#include <SPI.h>
#include <Fonts/TomThumb.h>
#include <ufbdisplay.hpp>
#include <inputs.hpp>
#include <atomic>

#define UFB_POWER 25

Inputs *inputs;
uint32_t input_buffer, output_buffer;
std::atomic<uint32_t> input_data, output_data;
std::atomic<uint8_t> current_profile;

void setup() {
  Serial.begin(9600);

  // Get the data variables initialized
  input_data.store(0);
  output_data.store(0);
  input_buffer = 0;
  output_buffer = 0;

  // Load the profiles from the SD card
  profiles[1] = {"Passthrough (1:1)"}; // No buttons get remapped
  current_profile.store(1);
  loadProfilesFromSDCard(profiles);

  // Clear all of the outputs and boot the UFB
  Serial.println("Starting controller...");

  inputs = new Inputs(&SPI);
  inputs->writeOutputs(&output_buffer);
  pinMode(UFB_POWER, OUTPUT);
  digitalWrite(UFB_POWER, HIGH);
}

void loop(){
  inputs->readInputs(&input_buffer);

  // Short circuit processing if the inputs haven't changed
  if (input_buffer == input_data.load()) return;

  // Switch profiles based on 31/32
  if (!(input_buffer >> 29 & 1)) {
    if (input_buffer >> 30 & 1) {
      int8_t prev_profile = current_profile.load() - 1;
      if (prev_profile < 1) return;
      if (profiles.count(prev_profile) > 0) {
        current_profile.store(prev_profile);
      }
    }

    if (input_buffer >> 31 & 1) {
      uint8_t next_profile = current_profile.load() + 1;
      if (profiles.count(next_profile) > 0) {
        current_profile.store(next_profile);
      }
    }
    
    input_data.store(input_buffer);
    return;
  }

  // Store and process input data
  input_data.store(input_buffer);
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
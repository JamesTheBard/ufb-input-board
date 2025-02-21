#ifndef INPUTS_H
#define INPUTS_H

#include <SPI.h>
#include <atomic>
#include <map>
#include <SD.h>
#include <ArduinoJson.h>
// #include <stdio.h>

#define SPI0_MISO  0
#define SPI0_SCLK  2
#define SPI0_MOSI  3
#define SPI0_SCLK_SPEED 10000000

#define INPUT_CE    21
#define INPUT_LATCH 20

#define OUTPUT_CE  1
#define OUTPUT_SS  6
#define OUTPUT_CLR 7

#define OUTPUT_TOTAL 17

#define SPI1_MISO  8
#define SPI1_SCLK 10
#define SPI1_MOSI 11

#define SDCARD_SS 12

class Profile {
    public:
        Profile(): profile_map() {}
        Profile(String new_name): profile_name(new_name), profile_map() {}
        Profile(String pname, std::map<uint8_t, uint32_t> pmap);

        String profile_name = "Unnamed Profile";
        std::map<uint8_t, uint32_t> profile_map;

        uint32_t processInputs(uint32_t data);
        void generateMask();

    private:
        bool is_passthrough = true;
        uint32_t mask;

};

uint32_t reverseBytes(uint32_t data);
bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles);

extern SPISettings inputSettings, outputSettings;
extern std::atomic<uint32_t> input_data;
extern std::atomic<uint32_t> output_data;
extern std::atomic<uint8_t> current_profile;
extern std::map<uint8_t, Profile> profiles;

#endif // INPUTS_H
#ifndef _PROFILES_HPP
#define _PROFILES_HPP

#include <SPI.h>
#include <atomic>
#include <map>
#include <SD.h>
#include <ArduinoJson.h>

#define SPI1_MISO  8
#define SPI1_SCLK 10
#define SPI1_MOSI 11

#define SDCARD_SS 12

#define OUTPUT_TOTAL 18

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

bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles);

extern std::map<uint8_t, Profile> profiles;

#endif // _PROFILES_HPP
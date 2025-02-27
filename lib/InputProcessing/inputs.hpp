#ifndef _INPUTS_HPP
#define _INPUTS_HPP

#include <SPI.h>
#include <atomic>
#include <map>

#define SPI0_MISO  0
#define SPI0_SCLK  2
#define SPI0_MOSI  3
#define SPI0_SCLK_SPEED_INPUTS  10000000
#define SPI0_SCLK_SPEED_OUTPUTS 10000000

#define INPUT_CE    21
#define INPUT_LATCH 20

#define OUTPUT_CE  1
#define OUTPUT_SS  6
#define OUTPUT_CLR 7

#define OUTPUT_TOTAL 18


extern SPISettings inputSettings, outputSettings;


class Profile {
    public:
        Profile(): profile_map() {}
        Profile(String new_name): profile_name(new_name), profile_map() {}
        Profile(String pname, std::map<uint8_t, uint32_t> pmap);

        String profile_name = "Unnamed Profile";
        uint8_t layout = 0;
        std::map<uint8_t, uint32_t> profile_map;

        uint32_t processInputs(const uint32_t data);
        void generateMask();

    private:
        bool is_passthrough = true;
        uint32_t mask;

};

uint32_t reverseBytes(const uint32_t data);

#endif // _INPUTS_HPP
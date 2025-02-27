#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <Arduino.h>
#include <map>
#include <atomic>
#include <SD.h>
#include <ArduinoJson.h>
#include "inputs.hpp"

#define SPI1_MISO  8
#define SPI1_SCLK 10
#define SPI1_MOSI 11

#define SDCARD_SS 12
#define DISP_DEFAULT_ADDR  0x3C

bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles, std::atomic<uint8_t> &display_addr);

#endif // _CONFIG_HPP
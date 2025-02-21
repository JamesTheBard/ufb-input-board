#ifndef _INPUTS_HPP
#define _INPUTS_HPP

#include <Arduino.h>
#include <SPI.h>
#include <atomic>
#include "profiles.hpp"

#define SPI0_MISO  0
#define SPI0_SCLK  2
#define SPI0_MOSI  3
#define SPI0_SCLK_READ  20000000
#define SPI0_SCLK_WRITE 10000000

#define INPUT_CE    21
#define INPUT_LATCH 20

#define OUTPUT_CE  1
#define OUTPUT_SS  6
#define OUTPUT_CLR 7

class Inputs {
    void initializePins();
    SPIClassRP2040 *inputsSPI;

public:
    Inputs();

    void readInputs(uint32_t *buffer);
    void writeOutputs(uint32_t *buffer);
};

uint32_t reverseBytes(uint32_t data);

extern SPISettings inputSettings, outputSettings;

#endif // _INPUTS_HPP
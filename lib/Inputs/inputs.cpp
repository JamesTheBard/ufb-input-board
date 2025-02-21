#include "inputs.hpp"

SPISettings inputSettings(SPI0_SCLK_READ, MSBFIRST, SPI_MODE2);
SPISettings outputSettings(SPI0_SCLK_WRITE, MSBFIRST, SPI_MODE0);

Inputs::Inputs() {
    SPI.setRX(SPI0_MISO);
    SPI.setRX(SPI0_MISO);
    SPI.setTX(SPI0_MOSI);
    SPI.setSCK(SPI0_SCLK);
    SPI.begin();

    initializePins();    
}

void Inputs::initializePins() {
    pinMode(INPUT_LATCH, OUTPUT);
    pinMode(INPUT_CE, OUTPUT);

    pinMode(OUTPUT_CE, OUTPUT);
    pinMode(OUTPUT_SS, OUTPUT);
    pinMode(OUTPUT_CLR, OUTPUT);

    digitalWrite(INPUT_LATCH, HIGH);
    digitalWrite(INPUT_CE, HIGH);
    digitalWrite(OUTPUT_CE, HIGH);
    digitalWrite(OUTPUT_CLR, HIGH);
}

void Inputs::readInputs(uint32_t *buffer) {
    SPI.beginTransaction(inputSettings);

    digitalWrite(INPUT_CE, LOW);
    digitalWrite(INPUT_LATCH, LOW);
    digitalWrite(INPUT_LATCH, HIGH);
    
    SPI.transfer(buffer, 4);
    
    digitalWrite(INPUT_CE, HIGH);
    
    SPI.endTransaction();
}

void Inputs::writeOutputs(uint32_t *buffer) {
    digitalWrite(OUTPUT_CE, LOW);
    digitalWrite(OUTPUT_SS, LOW);

    SPI.beginTransaction(outputSettings);
    SPI.transfer(buffer, 3);
    SPI.endTransaction();

    digitalWrite(OUTPUT_SS, HIGH);
    // digitalWrite(OUTPUT_CE, HIGH);
}

uint32_t reverseBytes(uint32_t data) {
    return ((data & 0xFF) << 24 | (data & 0xFF00) << 8 | (data & 0xFF0000) >> 8 | (data >> 24));
}

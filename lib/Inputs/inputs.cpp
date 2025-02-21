#include "inputs.hpp"

SPISettings inputSettings(SPI0_SCLK_READ, MSBFIRST, SPI_MODE2);
SPISettings outputSettings(SPI0_SCLK_WRITE, MSBFIRST, SPI_MODE0);

Inputs::Inputs(SPIClassRP2040 *spi) {
    SPIClassRP2040 *inputsSPI = spi;
    pinMode(INPUT_LATCH, OUTPUT);
    pinMode(INPUT_CE, OUTPUT);

    pinMode(OUTPUT_CE, OUTPUT);
    pinMode(OUTPUT_SS, OUTPUT);
    pinMode(OUTPUT_CLR, OUTPUT);

    digitalWrite(INPUT_LATCH, HIGH);
    digitalWrite(INPUT_CE, HIGH);
    digitalWrite(OUTPUT_CE, HIGH);
    digitalWrite(OUTPUT_CLR, HIGH);

    initializePins();
}

void Inputs::initializePins() {
    inputsSPI->setRX(SPI0_MISO);
    inputsSPI->setRX(SPI0_MISO);
    inputsSPI->setTX(SPI0_MOSI);
    inputsSPI->setSCK(SPI0_SCLK);
    inputsSPI->begin();
}

void Inputs::readInputs(uint32_t *buffer) {
    inputsSPI->beginTransaction(inputSettings);
    digitalWrite(INPUT_CE, LOW);
    digitalWrite(INPUT_LATCH, LOW);
    digitalWrite(INPUT_LATCH, HIGH);
    inputsSPI->transfer(buffer, 4);
    digitalWrite(INPUT_CE, HIGH);
    inputsSPI->endTransaction();
}

void Inputs::writeOutputs(uint32_t *buffer) {
    digitalWrite(OUTPUT_CE, LOW);
    digitalWrite(OUTPUT_SS, LOW);

    inputsSPI->beginTransaction(outputSettings);
    inputsSPI->transfer(buffer, 3);
    inputsSPI->endTransaction();

    digitalWrite(OUTPUT_SS, HIGH);
}

uint32_t reverseBytes(uint32_t data) {
    return ((data & 0xFF) << 24 | (data & 0xFF00) << 8 | (data & 0xFF0000) >> 8 | (data >> 24));
}

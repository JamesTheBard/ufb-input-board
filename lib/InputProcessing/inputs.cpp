#include "inputs.hpp"

SPISettings inputSettings(SPI0_SCLK_SPEED_INPUTS, MSBFIRST, SPI_MODE2);
SPISettings outputSettings(SPI0_SCLK_SPEED_OUTPUTS, MSBFIRST, SPI_MODE0);


Profile::Profile(String pname, std::map<uint8_t, uint32_t> pmap) {
    profile_name = pname;
    profile_map = pmap;
    generateMask();
}

/**
 * @brief Generate the default mask for the profile.
 */
void Profile::generateMask() {
    if (profile_map.empty()) {
        is_passthrough = true;
        return;
    }

    mask = (1 << OUTPUT_TOTAL) - 1;
    for (auto const &[key, val] : profile_map) {
        mask ^= 1 << (key - 1);
    }
    is_passthrough = false;
};

/**
 * Process all of the inputs with the associated masks.
 * 
 * @param data the input data
 * @return the processed output data
 */
uint32_t Profile::processInputs(const uint32_t data) {
    if (is_passthrough) return data;

    uint32_t processed_data = data & mask;
    for (auto const &[key, value] : profile_map) {
        processed_data |= value * (data >> (key - 1) & 1);    
    }
    return processed_data;
}

/**
 * Reverses the bytes of the given data.
 * 
 * @param data the data to reverse
 * @return the reversed data
 */
uint32_t reverseBytes(uint32_t data) {
    return ((data & 0xFF) << 24 | (data & 0xFF00) << 8 | (data & 0xFF0000) >> 8 | (data >> 24));
}

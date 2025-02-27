#include "config.hpp"

/**
 * Cleans up loading the profile post-error.
 * 
 * @param display_addr the display address to use
 */
void cleanup(std::atomic<uint8_t> &display_addr) {
    cleanup(display_addr);
    display_addr = DISP_DEFAULT_ADDR;
}

/**
 * Loads the profile configuration from the SD card.
 * 
 * @param profiles the profile map to load the profiles into
 * @return whether reading the profiles was successful
 */
bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles, std::atomic<uint8_t> &display_addr) {
    SPI1.setRX(SPI1_MISO);
    SPI1.setTX(SPI1_MOSI);
    SPI1.setSCK(SPI1_SCLK);

    if (!SD.begin(SDCARD_SS, SPI1)) {
        Serial.println("SDCard is has either failed or is not present, skipping.");
        cleanup(display_addr);
        return false;
    }

    File pfile = SD.open("profiles.json");
    if (!pfile) { 
        Serial.println("Could not open the profiles configuration 'profiles.json', skipping.");
        cleanup(display_addr);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pfile);
    if (error) {
        Serial.print(F("Deserializating profile data failed: "));
        Serial.println(error.f_str());
        pfile.close();
        cleanup(display_addr);
        return false;
    }

    Serial.println("Loading profiles...");

    uint8_t default_layout = 0;
    JsonObject display_config = doc["display"];
    if (display_config != NULL) {
        if (display_config["address"].is<String>()) {
            String s = display_config["address"];
            if (s.startsWith("0x") || s.startsWith("0X")) {
                s = s.substring(2);
            }
            display_addr.store((uint8_t)strtoul(s.c_str(), NULL, 16));
        } else {
            display_addr.store(DISP_DEFAULT_ADDR);
        }

        if (display_config["default_layout"].is<uint8_t>()) {
            default_layout = display_config["default_layout"];
            profiles[1].layout = default_layout;
        }
    }
  
    uint8_t pcount = 2;
    for (JsonObject pobj : doc["profiles"].as<JsonArray>()) {
        if (pcount > 9) break;
        profiles[pcount] = Profile();
        profiles[pcount].layout = default_layout;
        for (JsonPair kv : pobj) {
            if (String(kv.key().c_str()) == "name") {
                profiles[pcount].profile_name = kv.value().as<String>();
            }
            if (String(kv.key().c_str()) == "mappings") {
                for (JsonArray marray : kv.value().as<JsonArray>()) {
                    if (marray[0].as<uint8_t>() >= 30) continue; // ignore profile button remaps (inputs 30-32)
                    uint32_t output_mask = 0;
                    for (uint8_t output : marray[1].as<JsonArray>()) {
                        if (output > OUTPUT_TOTAL) continue;
                        output_mask ^= (1 << (output - 1));
                    }
                    profiles[pcount].profile_map[marray[0].as<uint8_t>()] = output_mask;
                }
            }
            if (String(kv.key().c_str()) == "layout") {
                profiles[pcount].layout = kv.value().as<uint8_t>();
            }
        }
        profiles[pcount].generateMask();
        pcount++;
    }

    pfile.close();
    SPI1.end();
    return true;
}

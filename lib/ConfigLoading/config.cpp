#include "config.hpp"

/**
 * Cleans up loading the profile post-error.
 * 
 * @param display_addr the display address to use
 */
void cleanup() {
    SPI1.end();
}

/**
 * Loads the profile configuration from the SD card.
 * 
 * @param profiles the profile map to load the profiles into
 * @return whether reading the profiles was successful
 */
bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles, DisplayConfig &display_config) {
    display_config.address = DISP_DEFAULT_ADDR;

    SPI1.setRX(SPI1_MISO);
    SPI1.setTX(SPI1_MOSI);
    SPI1.setSCK(SPI1_SCLK);

    if (!SD.begin(SDCARD_SS, SPI1)) {
        Serial.println("SDCard is has either failed or is not present, skipping.");
        cleanup();
        return true;
    }

    File pfile = SD.open("profiles.json");
    if (!pfile) { 
        Serial.println("Could not open the profiles configuration 'profiles.json', skipping.");
        cleanup();
        return true;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pfile);
    if (error) {
        Serial.print(F("Deserializating profile data failed: "));
        Serial.println(error.f_str());
        pfile.close();
        cleanup();
        return true;
    }

    Serial.println("Loading profiles...");

    uint8_t default_layout = 0;
    JsonObject dconfig = doc["display"];
    if (dconfig != NULL) {
        if (dconfig["address"].is<String>()) {
            String s = dconfig["address"];
            if (s.startsWith("0x") || s.startsWith("0X")) {
                s = s.substring(2);
            }
            display_config.address.store((uint8_t)strtoul(s.c_str(), NULL, 16));
        }

        if (dconfig["default_layout"].is<uint8_t>()) {
            default_layout = dconfig["default_layout"];
            profiles[1].layout = default_layout;
        }

        if (dconfig["type"].is<String>()) {
            display_config.type = dconfig["type"].as<String>();
        }

        if (dconfig["resolution"].is<String>()) {
            display_config.resolution = dconfig["resolution"].as<String>();
            display_config.resolution.toLowerCase();
        }
    }
  
    uint8_t pcount = 2;
    for (JsonObject pobj : doc["profiles"].as<JsonArray>()) {
        if (pcount > 9) break;
        profiles[pcount] = Profile();
        profiles[pcount].layout = default_layout;
        for (JsonPair kv : pobj) {
            if (kv.key() == "name") {
                profiles[pcount].profile_name = kv.value().as<String>();
            }
            if (kv.key() == "mappings") {
                for (JsonArray marray : kv.value().as<JsonArray>()) {
                    if (!marray[0].is<uint8_t>()) continue;

                    const uint8_t input_id = marray[0].as<uint8_t>();
                    if (input_id >= 30 || input_id == 0) continue; // ignore profile button remaps (inputs 30-32)

                    uint32_t output_mask = 0;
                    for (uint8_t output : marray[1].as<JsonArray>()) {
                        if (output > OUTPUT_TOTAL) continue;
                        output_mask ^= (1 << (output - 1));
                    }
                    profiles[pcount].profile_map[input_id] = output_mask;
                }
            }
            if (kv.key() == "layout") {
                if (kv.value().is<uint8_t>())
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

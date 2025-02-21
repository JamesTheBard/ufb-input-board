#include "profiles.hpp"

Profile::Profile(String pname, std::map<uint8_t, uint32_t> pmap) {
    profile_name = pname;
    profile_map = pmap;
    generateMask();
}

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

uint32_t Profile::processInputs(uint32_t data) {
    if (is_passthrough) { return data; };

    uint32_t processed_data = data & mask;
    for (auto const &[key, value] : profile_map) {
        processed_data |= value * (data >> (key - 1) & 1);    
    }
    return processed_data;
}

bool loadProfilesFromSDCard(std::map<uint8_t, Profile> &profiles) {
    SPI1.setRX(SPI1_MISO);
    SPI1.setTX(SPI1_MOSI);
    SPI1.setSCK(SPI1_SCLK);

    if (!SD.begin(SDCARD_SS, SPI1)) {
        Serial.println("SDCard is has either failed or is not present, skipping.");
        return false;
        SPI1.end();
    }

    File pfile = SD.open("profiles.json");
    if (!pfile) { 
        Serial.println("Could not open the profiles configuration 'profiles.json', skipping.");
        return false;
        SPI1.end();
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pfile);
    if (error) {
        Serial.print(F("Deserializating profile data failed: "));
        Serial.println(error.f_str());
        pfile.close();
        SPI1.end();
        return false;
    }

    uint8_t pcount = 2;
    Serial.println("Loading profiles...");
    for (JsonObject pobj : doc["profiles"].as<JsonArray>()) {
        profiles[pcount] = Profile();
        for (JsonPair kv : pobj) {
            if (String(kv.key().c_str()) == "name") {
                profiles[pcount].profile_name = kv.value().as<String>();
            }
            if (String(kv.key().c_str()) == "mappings") {
                for (JsonArray marray : kv.value().as<JsonArray>()) {
                    const char *v = marray[1];
                    profiles[pcount].profile_map[marray[0].as<uint8_t>()] = std::stoul(v, nullptr, 16);
                }
            }
        }
        profiles[pcount].generateMask();
        pcount++;
    }
  
    pfile.close();
    SPI1.end();
    return true;
}

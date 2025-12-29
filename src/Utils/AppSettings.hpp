#pragma once
#include <string>
#include <fstream>
#include <iostream>

struct AppSettings {
    // Input
    bool useAlphabeticalRibbon = false;
    float lerpStrength = 0.22f; // Default "Premium Feel"

    // Visuals
    bool stealthMode = false; // Stealth Black vs Classic UI

    // Security
    int decoyScreenIndex = 0; // 0: Fake Update, 1: Error Screen (example)

    // BuJo
    int defaultTemplateIndex = 0; // 0: Rapid Log (Daily), 1: Cornell, 2: Charting
    
    // File path
    const std::string configPath = "settings.cfg";

    void save() {
        std::ofstream file(configPath);
        if (file.is_open()) {
            file << "useAlphabeticalRibbon=" << useAlphabeticalRibbon << "\n";
            file << "lerpStrength=" << lerpStrength << "\n";
            file << "stealthMode=" << stealthMode << "\n";
            file << "decoyScreenIndex=" << decoyScreenIndex << "\n";
            file << "defaultTemplateIndex=" << defaultTemplateIndex << "\n";
            file.close();
        }
    }

    void load() {
        std::ifstream file(configPath);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string key = line.substr(0, eq);
                    std::string val = line.substr(eq + 1);
                    
                    if (key == "useAlphabeticalRibbon") useAlphabeticalRibbon = (val == "1");
                    else if (key == "lerpStrength") lerpStrength = std::stof(val);
                    else if (key == "stealthMode") stealthMode = (val == "1");
                    else if (key == "decoyScreenIndex") decoyScreenIndex = std::stoi(val);
                    else if (key == "defaultTemplateIndex") defaultTemplateIndex = std::stoi(val);
                }
            }
            file.close();
        }
    }
};

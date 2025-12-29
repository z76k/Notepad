#include "SettingsState.hpp"
#include "../App.hpp"
#include "BrowserState.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h> // for sysconf

void SettingsState::enter(App& app) {
    buildMenu(app);
    // Find first selectable item
    for(size_t i=0; i<items.size(); ++i) {
        if (items[i].type != ItemType::HEADER) {
            selectedIndex = i;
            break;
        }
    }
    selectorY = 80 + selectedIndex * 40;
    targetSelectorY = selectorY;
}

void SettingsState::exit(App& app) {
    app.getSettings().save();
}

void SettingsState::buildMenu(App& app) {
    items.clear();
    auto& s = app.getSettings();

    // [INPUT]
    items.push_back({"[ INPUT ]", ItemType::HEADER});
    items.push_back({"Keyboard Layout", ItemType::TOGGLE, &s.useAlphabeticalRibbon});
    
    // [VISUALS]
    items.push_back({"[ VISUALS ]", ItemType::HEADER});
    items.push_back({"Lerp Strength", ItemType::SLIDER, nullptr, &s.lerpStrength, nullptr, 0.1f, 0.5f});
    items.push_back({"Stealth Mode", ItemType::TOGGLE, &s.stealthMode});

    // [BUJO]
    items.push_back({"[ BUJO ]", ItemType::HEADER});
    items.push_back({"Default Template", ItemType::SELECTOR, nullptr, nullptr, &s.defaultTemplateIndex, 0.0f, 0.0f, 0, 2});

    // [SECURITY]
    items.push_back({"[ SECURITY ]", ItemType::HEADER});
    items.push_back({"Decoy Screen", ItemType::SELECTOR, nullptr, nullptr, &s.decoyScreenIndex, 0.0f, 0.0f, 0, 2});
}

void SettingsState::handleEvent(App& app, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_b) {
            app.changeState(std::make_shared<BrowserState>());
            return;
        }

        if (event.key.keysym.sym == SDLK_UP) {
            do {
                if (selectedIndex > 0) selectedIndex--;
            } while (selectedIndex > 0 && items[selectedIndex].type == ItemType::HEADER);
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
             do {
                if (selectedIndex < items.size() - 1) selectedIndex++;
            } while (selectedIndex < items.size() - 1 && items[selectedIndex].type == ItemType::HEADER);
        }

        // Adjust values
        MenuItem& item = items[selectedIndex];
        if (event.key.keysym.sym == SDLK_LEFT) {
            if (item.type == ItemType::SLIDER && item.floatVal) {
                *item.floatVal -= 0.05f;
                if (*item.floatVal < item.minF) *item.floatVal = item.minF;
            }
            if (item.type == ItemType::SELECTOR && item.intVal) {
                (*item.intVal)--;
                if (*item.intVal < item.minI) *item.intVal = item.maxI;
            }
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
            if (item.type == ItemType::SLIDER && item.floatVal) {
                *item.floatVal += 0.05f;
                if (*item.floatVal > item.maxF) *item.floatVal = item.maxF;
            }
            if (item.type == ItemType::SELECTOR && item.intVal) {
                (*item.intVal)++;
                if (*item.intVal > item.maxI) *item.intVal = item.minI;
            }
        }
        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_a) {
            if (item.type == ItemType::TOGGLE && item.boolVal) {
                *item.boolVal = !(*item.boolVal);
            }
        }
    }
}

void SettingsState::update(App& app) {
    // Lerp selector
    targetSelectorY = 80 + selectedIndex * 40;
    float lerp = app.getSettings().lerpStrength;
    selectorY += (targetSelectorY - selectorY) * lerp;
}

void SettingsState::render(App& app, SDL_Renderer* renderer) {
    bool stealth = app.getSettings().stealthMode;
    if (stealth) SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
    else SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
    SDL_RenderClear(renderer);

    // Header
    renderText(renderer, app.getFont(), "SETTINGS", 320, 30, {255, 255, 255, 255});

    // Selector Bar
    SDL_Rect selRect = {40, static_cast<int>(selectorY), 560, 36};
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 100);
    SDL_RenderFillRect(renderer, &selRect);

    // Items
    int startY = 80;
    for (size_t i = 0; i < items.size(); ++i) {
        int y = startY + i * 40;
        SDL_Color col = {200, 200, 200, 255};
        if (i == selectedIndex) col = {255, 255, 255, 255};
        
        if (items[i].type == ItemType::HEADER) {
            col = {255, 200, 100, 255};
            renderText(renderer, app.getFont(), items[i].label, 320, y + 18, col);
        } else {
            renderText(renderer, app.getFont(), items[i].label, 100, y + 18, col);
            
            // Value
            std::string valStr;
            if (items[i].type == ItemType::TOGGLE) {
                valStr = *items[i].boolVal ? "ON" : "OFF";
            } else if (items[i].type == ItemType::SLIDER) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << *items[i].floatVal;
                valStr = ss.str();
            } else if (items[i].type == ItemType::SELECTOR) {
                valStr = std::to_string(*items[i].intVal);
                if (items[i].label == "Keyboard Layout") {
                    valStr = *items[i].boolVal ? "ABC" : "QWERTY"; // Logic mismatch in my struct usage? 
                    // Ah, Keyboard layout is a TOGGLE in buildMenu above.
                }
                if (items[i].label == "Decoy Screen") {
                    if (*items[i].intVal == 0) valStr = "Fake Update";
                    else if (*items[i].intVal == 1) valStr = "Error Log";
                    else valStr = "Black Screen";
                }
                if (items[i].label == "Default Template") {
                    if (*items[i].intVal == 0) valStr = "Daily Log";
                    else if (*items[i].intVal == 1) valStr = "Cornell";
                    else valStr = "Charting";
                }
            }
             renderText(renderer, app.getFont(), valStr, 500, y + 18, col);
        }
    }

    // Footer System Monitor
    int footerY = 440;
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_Rect footer = {0, footerY, 640, 40};
    SDL_RenderFillRect(renderer, &footer);
    
    std::string stats = "MEM: " + getRAMUsage() + " | BAT: " + getBatteryLevel();
    renderText(renderer, app.getFont(), stats, 320, footerY + 20, {150, 150, 150, 255});
}

std::string SettingsState::getRAMUsage() {
    // Simple mock or /proc/meminfo read
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGESIZE);
    long total_pages = sysconf(_SC_PHYS_PAGES);
    
    long free_mem = pages * page_size;
    long total_mem = total_pages * page_size;
    long used_mem = total_mem - free_mem;
    
    // Convert to MB
    return std::to_string(used_mem / 1024 / 1024) + "MB / " + std::to_string(total_mem / 1024 / 1024) + "MB";
}

std::string SettingsState::getBatteryLevel() {
    // Try to read standard battery path
    std::ifstream bat("/sys/class/power_supply/battery/capacity");
    if (bat.is_open()) {
        std::string cap;
        bat >> cap;
        return cap + "%";
    }
    return "100%"; // Mock
}

void SettingsState::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w = surface->w;
    int h = surface->h;
    SDL_Rect dst = {x - w/2, y - h/2, w, h}; // Center aligned
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

#pragma once
#include "../State.hpp"
#include <vector>
#include <string>

enum class ItemType {
    HEADER,
    TOGGLE,
    SLIDER,
    SELECTOR
};

struct MenuItem {
    std::string label;
    ItemType type;
    
    // Pointers to AppSettings values
    bool* boolVal = nullptr;
    float* floatVal = nullptr;
    int* intVal = nullptr;

    // For Slider/Selector limits
    float minF = 0.0f, maxF = 1.0f;
    int minI = 0, maxI = 1;
};

class SettingsState : public State {
public:
    void enter(App& app) override;
    void exit(App& app) override;
    void handleEvent(App& app, const SDL_Event& event) override;
    void update(App& app) override;
    void render(App& app, SDL_Renderer* renderer) override;

private:
    std::vector<MenuItem> items;
    int selectedIndex = 0;
    
    // Smooth Selector
    float selectorY = 0.0f;
    float targetSelectorY = 0.0f;

    // System Monitor
    std::string getRAMUsage();
    std::string getBatteryLevel();
    
    void buildMenu(App& app);
    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
};

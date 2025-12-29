#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include <functional>

class InputEngine {
public:
    InputEngine(TTF_Font* font);
    ~InputEngine();

    // Returns true if input was generated
    bool handleEvent(const SDL_Event& event);
    void update();
    void render(SDL_Renderer* renderer);

    // Access the last generated input and clear it
    std::string popInput();
    bool hasInput() const { return !inputBuffer.empty(); }

    // Set dictionary for predictions
    void setDictionary(const std::vector<std::string>& words);

    // Configuration
    void setLerpStrength(float strength) { lerpStrength = strength; }
    void setKeyboardLayout(bool alphabetical);

private:
    static constexpr int SCREEN_WIDTH = 640;
    static constexpr int SCREEN_HEIGHT = 480;
    static constexpr int CHAR_WIDTH = 40;
    static constexpr int WORD_HEIGHT = 30;
    static constexpr int RIBBON_Y = 400;
    static constexpr int CRANK_X = 540;

    TTF_Font* font;
    
    // Settings
    float lerpStrength = 0.22f;
    std::string qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM "; 
    std::vector<std::string> predictions;
    std::string inputBuffer; // Accumulates characters to be popped by Editor

    // State
    enum class Focus {
        RIBBON,
        CRANK
    };
    Focus currentFocus = Focus::RIBBON;

    // Ribbon Physics
    float ribbonVisualPos = 0.0f;
    float ribbonTargetPos = 0.0f;
    int ribbonIndex = 0;

    // Crank Physics
    float crankVisualPos = 0.0f;
    float crankTargetPos = 0.0f;
    int crankIndex = 0;

    // Helper methods
    void updatePhysics();
    void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color);
    
    void updatePredictions();
};

#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <vector>
#include <iostream>
#include "State.hpp"
#include "Utils/AppSettings.hpp"

class App {
public:
    App();
    ~App();

    bool init();
    void run();
    void changeState(std::shared_ptr<State> newState);
    
    // Accessors
    SDL_Renderer* getRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font; }
    int getScreenWidth() const { return SCREEN_WIDTH; }
    int getScreenHeight() const { return SCREEN_HEIGHT; }
    AppSettings& getSettings() { return settings; }

    // Global Input Handling (Konami, Panic)
    void checkGlobalInput(const SDL_Event& event);
    bool isVaultUnlocked() const { return vaultUnlocked; }

private:
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    bool running = true;
    std::shared_ptr<State> currentState;

    // Konami Code Logic
    std::vector<SDL_Keycode> konamiCode = {
        SDLK_UP, SDLK_UP, SDLK_DOWN, SDLK_DOWN, 
        SDLK_LEFT, SDLK_RIGHT, SDLK_LEFT, SDLK_RIGHT, 
        SDLK_b, SDLK_a
    };
    size_t konamiIndex = 0;
    bool vaultUnlocked = false;

    AppSettings settings;
};

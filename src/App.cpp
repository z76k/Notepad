#include "App.hpp"
#include "States/BrowserState.hpp"
#include "States/DecoyState.hpp"
#include "Utils/FileSystem.hpp"
#include <iostream>

App::App() {
    settings.load();
}

App::~App() {
    settings.save();
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

bool App::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;

    window = SDL_CreateWindow("Miyoo Notes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) return false;

    // Load Font
    font = TTF_OpenFont("assets/fonts/default.ttf", 20);
    if (!font) font = TTF_OpenFont("../assets/fonts/default.ttf", 20); // Try relative
    if (!font) {
        std::cerr << "Failed to load font!" << std::endl;
        return false;
    }

    FileSystem::init();

    // Start in Browser State
    changeState(std::make_shared<BrowserState>());

    return true;
}

void App::changeState(std::shared_ptr<State> newState) {
    if (currentState) currentState->exit(*this);
    currentState = newState;
    if (currentState) currentState->enter(*this);
}

void App::checkGlobalInput(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // Konami Code
        if (event.key.keysym.sym == konamiCode[konamiIndex]) {
            konamiIndex++;
            if (konamiIndex >= konamiCode.size()) {
                vaultUnlocked = !vaultUnlocked; // Toggle
                konamiIndex = 0;
                std::cout << "Vault Unlocked: " << vaultUnlocked << std::endl;
                // Force refresh current state if it's browser
                // A better way is to use an event bus, but here we can just reload browser
                if (std::dynamic_pointer_cast<BrowserState>(currentState)) {
                    changeState(std::make_shared<BrowserState>());
                }
            }
        } else {
            konamiIndex = 0;
        }

        // Panic Decoy: L2 (assumed K) + R2 (assumed L) + SELECT (Shift) + START (Return)
        // This is hard to detect simultaneously with just KeyDown unless we track state.
        // For simplicity, we use a specific panic key for dev: 'P' or verify modifiers.
        // Let's implement robust modifier check.
        const Uint8* state = SDL_GetKeyboardState(NULL);
        // Assuming mapping: L2=k, R2=l, SELECT=LSHIFT, START=RETURN
        if (state[SDL_SCANCODE_K] && state[SDL_SCANCODE_L] && 
            (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) && 
            state[SDL_SCANCODE_RETURN]) {
            changeState(std::make_shared<DecoyState>(settings.decoyScreenIndex));
        }
    }
}

void App::run() {
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) running = false;
            
            checkGlobalInput(e);
            if (currentState) currentState->handleEvent(*this, e);
        }

        if (currentState) currentState->update(*this);

        if (currentState) currentState->render(*this, renderer);
        SDL_RenderPresent(renderer);
        
        SDL_Delay(16);
    }
}

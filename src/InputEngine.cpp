#include "InputEngine.hpp"
#include <iostream>

InputEngine::InputEngine(TTF_Font* font) : font(font) {
    // Initialize predictions with some dummy data
    predictions = {"HELLO", "WORLD", "MIYOO", "MINI", "PLUS", "LINUX", "SDL2", "CODE", "RETRO", "GAMING"};
    updatePredictions();
}

InputEngine::~InputEngine() {
}

void InputEngine::setDictionary(const std::vector<std::string>& words) {
    predictions = words;
    if (predictions.empty()) predictions.push_back("...");
    crankIndex = 0;
}

std::string InputEngine::popInput() {
    std::string s = inputBuffer;
    inputBuffer.clear();
    return s;
}

void InputEngine::updatePredictions() {
    // In a real engine, this would filter based on current input
    // For now, we just keep the static list or cycle them
}

bool InputEngine::handleEvent(const SDL_Event& event) {
    bool handled = false;
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                if (currentFocus == Focus::RIBBON) {
                    ribbonIndex--;
                    if (ribbonIndex < 0) ribbonIndex = qwerty.length() - 1;
                    handled = true;
                }
                break;
            case SDLK_RIGHT:
                if (currentFocus == Focus::RIBBON) {
                    ribbonIndex++;
                    if (ribbonIndex >= qwerty.length()) ribbonIndex = 0;
                    handled = true;
                }
                break;
            case SDLK_UP:
                if (currentFocus == Focus::CRANK) {
                    crankIndex--;
                    if (crankIndex < 0) crankIndex = predictions.size() - 1;
                    handled = true;
                }
                break;
            case SDLK_DOWN:
                if (currentFocus == Focus::CRANK) {
                    crankIndex++;
                    if (crankIndex >= predictions.size()) crankIndex = 0;
                    handled = true;
                }
                break;
            
            case SDLK_q: // L1
                if (currentFocus == Focus::RIBBON) {
                    ribbonIndex -= 5;
                    if (ribbonIndex < 0) ribbonIndex += qwerty.length();
                    if (ribbonIndex < 0) ribbonIndex = 0; 
                    handled = true;
                }
                break;
            case SDLK_e: // R1
                if (currentFocus == Focus::RIBBON) {
                    ribbonIndex += 5;
                    if (ribbonIndex >= qwerty.length()) ribbonIndex -= qwerty.length();
                    handled = true;
                }
                break;

            case SDLK_RETURN: // START
                if (currentFocus == Focus::RIBBON) {
                    inputBuffer += qwerty[ribbonIndex];
                } else {
                    if (!predictions.empty()) {
                        inputBuffer += predictions[crankIndex] + " ";
                    }
                }
                handled = true;
                break;

            case SDLK_RSHIFT: // SELECT
            case SDLK_LSHIFT:
                currentFocus = (currentFocus == Focus::RIBBON) ? Focus::CRANK : Focus::RIBBON;
                handled = true;
                break;
        }
    }
    return handled;
}

void InputEngine::update() {
    // Update Targets based on indices
    // Ribbon: Horizontal
    float ribbonCenterOffset = (SCREEN_WIDTH / 2) - (CHAR_WIDTH / 2);
    ribbonTargetPos = ribbonCenterOffset - (ribbonIndex * CHAR_WIDTH);

    // Crank: Vertical
    float crankCenterOffset = (SCREEN_HEIGHT / 2) - (WORD_HEIGHT / 2);
    crankTargetPos = crankCenterOffset - (crankIndex * WORD_HEIGHT);

    updatePhysics();
}

void InputEngine::setKeyboardLayout(bool alphabetical) {
    if (alphabetical) {
        qwerty = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    } else {
        qwerty = "QWERTYUIOPASDFGHJKLZXCVBNM ";
    }
}

void InputEngine::updatePhysics() {
    // VisualPos+=(TargetPos−VisualPos)×lerpStrength
    ribbonVisualPos += (ribbonTargetPos - ribbonVisualPos) * lerpStrength;
    crankVisualPos += (crankTargetPos - crankVisualPos) * lerpStrength;
}

void InputEngine::render(SDL_Renderer* renderer) {
    // Do NOT clear background here, allowing overlays
    // Render Ribbon (Horizontal)
    int startY = RIBBON_Y;
    
    // Draw selection box for Ribbon
    if (currentFocus == Focus::RIBBON) {
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 100); 
        SDL_Rect selRect = {(SCREEN_WIDTH - CHAR_WIDTH)/2, startY, CHAR_WIDTH, CHAR_WIDTH}; 
        SDL_RenderFillRect(renderer, &selRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_Rect selRect = {(SCREEN_WIDTH - CHAR_WIDTH)/2, startY, CHAR_WIDTH, CHAR_WIDTH};
        SDL_RenderDrawRect(renderer, &selRect);
    }

    for (size_t i = 0; i < qwerty.length(); ++i) {
        float x = ribbonVisualPos + (i * CHAR_WIDTH);
        if (x < -CHAR_WIDTH || x > SCREEN_WIDTH) continue;

        std::string s(1, qwerty[i]);
        SDL_Color col = {200, 200, 200, 255};
        if (i == ribbonIndex) col = {255, 255, 255, 255};
        
        renderText(renderer, s, static_cast<int>(x) + 10, startY + 5, col);
    }

    // Render Crank (Vertical) on Right
    int crankX = CRANK_X;
    
    // Draw selection box for Crank
    if (currentFocus == Focus::CRANK) {
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 100);
        SDL_Rect selRect = {crankX - 10, (SCREEN_HEIGHT - WORD_HEIGHT)/2, 100, WORD_HEIGHT};
        SDL_RenderFillRect(renderer, &selRect);
    }

    for (size_t i = 0; i < predictions.size(); ++i) {
        float y = crankVisualPos + (i * WORD_HEIGHT);
        if (y < -WORD_HEIGHT || y > SCREEN_HEIGHT) continue;

        SDL_Color col = {150, 150, 150, 255};
        if (i == crankIndex) col = {255, 255, 255, 255};

        renderText(renderer, predictions[i], crankX, static_cast<int>(y), col);
    }
}

void InputEngine::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    if (text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

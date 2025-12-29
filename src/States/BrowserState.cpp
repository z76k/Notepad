#include "BrowserState.hpp"
#include "EditorState.hpp"
#include "CanvasState.hpp"
#include "SettingsState.hpp"
#include <iostream>

void BrowserState::enter(App& app) {
    fileList = FileSystem::listFiles(app.isVaultUnlocked());
    selectedIndex = 0;
    std::cout << "Entered Browser State" << std::endl;
}

void BrowserState::exit(App& app) {
    // Cleanup if needed
}

void BrowserState::handleEvent(App& app, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                selectedIndex--;
                if (selectedIndex < 0) selectedIndex = fileList.size() - 1;
                break;
            case SDLK_DOWN:
                selectedIndex++;
                if (selectedIndex >= fileList.size()) selectedIndex = 0;
                break;
            case SDLK_a: // Open
            case SDLK_RETURN: // START: Open File
                if (!fileList.empty()) {
                    app.changeState(std::make_shared<EditorState>(fileList[selectedIndex]));
                }
                break;
            case SDLK_x: // New File (Editor)
                app.changeState(std::make_shared<EditorState>(""));
                break;
            case SDLK_c: // New Canvas
                 app.changeState(std::make_shared<CanvasState>());
                 break;
            case SDLK_y: // Privatize (Simulated 'Y' button)
                if (!fileList.empty()) {
                    std::string file = fileList[selectedIndex];
                    if (file.find("[LOCKED]") == std::string::npos) {
                        FileSystem::privatizeFile(file);
                        // Refresh list
                        fileList = FileSystem::listFiles(app.isVaultUnlocked());
                    }
                }
                break;
            case SDLK_BACKSPACE: // Select / Settings
                app.changeState(std::make_shared<SettingsState>());
                break;
        }
    }
}

void BrowserState::update(App& app) {
    // Animation logic if any
}

void BrowserState::render(App& app, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = app.getFont();
    
    renderText(renderer, font, "FILE BROWSER", 20, 20, {255, 200, 100, 255});
    
    int startY = 80;
    int lineHeight = 40;

    for (size_t i = 0; i < fileList.size(); ++i) {
        SDL_Color col = {150, 150, 150, 255};
        if (i == selectedIndex) col = {255, 255, 255, 255};
        
        renderText(renderer, font, fileList[i], 40, startY + (i * lineHeight), col);
    }

    if (fileList.empty()) {
        renderText(renderer, font, "No files found. Press START to create new.", 40, 200, {100, 100, 100, 255});
    }
}

void BrowserState::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    if (text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

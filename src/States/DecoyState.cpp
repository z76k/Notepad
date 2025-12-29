#include "DecoyState.hpp"
#include "../App.hpp"
#include <cstdlib>

DecoyState::DecoyState(int mode) : mode(mode) {}

void DecoyState::enter(App& app) {
    if (mode == 0) { // Fake Update
        logLines = {
            "[SYSTEM] Kernel Panic: VFS unable to mount root fs on unknown-block(0,0)",
            "[CRITICAL] Memory integrity check failed at 0x004F3A",
            "Loading recovery modules...",
            "module: ext4... OK",
            "module: fat32... OK",
            "Checking block descriptors...",
            "Update found: firmware_v4.2.0.bin",
            "Verifying signature... OK",
            "Writing to flash... 0%",
            "Writing to flash... 4%",
            "Writing to flash... 9%",
            "DO NOT POWER OFF",
            "Saving system state...",
            "Backing up registry...",
            "Critical Error: 0x8004005",
            "Retrying operation...",
            "Sector 4522 corrupted, attempting repair...",
        };
    } else if (mode == 1) { // Error Log
        logLines = {
             "Error: SEGFAULT at 0x00000000",
             "Core dumped.",
             "Stack trace:",
             "  <0x00401234> main + 0x23",
             "  <0x00405678> unknown_function + 0x12",
             "  <0x7f345678> libc.so.6 + 0x45",
             "System halted.",
             "Contact administrator.",
        };
    }
    // Mode 2 is Black Screen (Empty logs)
}

void DecoyState::exit(App& app) {}

void DecoyState::handleEvent(App& app, const SDL_Event& event) {
    // Consume all input. No escape.
}

void DecoyState::update(App& app) {
    if (mode != 2) scrollOffset += 0.5f; // Auto scroll
}

void DecoyState::render(App& app, SDL_Renderer* renderer) {
    if (mode == 2) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = app.getFont();
    
    // Warning Header
    if (mode == 0) {
        SDL_Color red = {255, 50, 50, 255};
        SDL_Surface* surf = TTF_RenderText_Blended(font, "CRITICAL UPDATE: SAVING SYSTEM STATE", red);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {40, 40, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }

        surf = TTF_RenderText_Blended(font, "DO NOT POWER OFF", red);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {40, 80, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    } else if (mode == 1) {
        SDL_Color red = {255, 0, 0, 255};
        SDL_Surface* surf = TTF_RenderText_Blended(font, "FATAL ERROR", red);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect dst = {40, 40, surf->w, surf->h};
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    // Scrolling Log
    int startY = 150;
    int lineHeight = 25;
    SDL_Color green = {50, 255, 50, 255};
    if (mode == 1) green = {200, 200, 200, 255};

    for (size_t i = 0; i < logLines.size(); ++i) {
        float y = startY + (i * lineHeight) - scrollOffset;
        
        // Loop log logic simplified: just let it scroll
        
        if (y > 100 && y < 480) {
            SDL_Surface* surf = TTF_RenderText_Blended(font, logLines[i].c_str(), green);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {40, (int)y, surf->w, surf->h};
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
    }
}

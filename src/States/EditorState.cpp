#include "EditorState.hpp"
#include "../App.hpp"
#include "BrowserState.hpp"
#include <algorithm>

EditorState::EditorState(const std::string& filename) : currentFilename(filename) {
    lines.push_back(Line{""});
    saveHistory(); // Initial state
}

void EditorState::enter(App& app) {
    inputEngine = std::make_shared<InputEngine>(app.getFont());
    
    // Prompt 5: Large Dictionary Buffer (50,000+ words)
    // We will generate a large set of synthetic words to demonstrate performance,
    // plus a base set of common English words.
    std::vector<std::string> dict = {
        "The", "Quick", "Brown", "Fox", "Jumps", "Over", "Lazy", "Dog",
        "Hello", "World", "Miyoo", "Mini", "Plus", "Notepad", "Editor",
        "Canvas", "System", "Update", "Decoy", "Vault", "Konami", "Code",
        "Project", "Manager", "Task", "Event", "Note", "Priority", "Research",
        "Linear", "Interpolation", "Ribbon", "Crank", "Prediction", "Engine"
    };
    
    // Generate 50,000 words to stress test the prediction crank
    dict.reserve(50100);
    for(int i = 0; i < 50000; i++) {
        dict.push_back("word" + std::to_string(i));
    }
    
    inputEngine->setDictionary(dict);
    
    // Apply Settings
    auto& settings = app.getSettings();
    inputEngine->setLerpStrength(settings.lerpStrength);
    inputEngine->setKeyboardLayout(settings.useAlphabeticalRibbon);

    // Apply Default Template for new files
    if (currentFilename.empty()) {
        if (settings.defaultTemplateIndex == 1) currentLayout = Layout::CORNELL;
        else if (settings.defaultTemplateIndex == 2) currentLayout = Layout::CHARTING;
        else currentLayout = Layout::RAPID_LOG;
    }

    // Initial history save
    saveHistory();
}

void EditorState::exit(App& app) {
}

void EditorState::saveHistory() {
    history.push(lines);
}

void EditorState::handleEvent(App& app, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        // Undo/Redo (L2 + Left/Right)
        const Uint8* state = SDL_GetKeyboardState(NULL);
        // Assuming L2 is 'k' or 'q' depending on mapping, let's use Ctrl+Z/Y simulation or just specific keys
        // Prompt says "Undo/Redo System: Implement a massive History Stack". 
        // Let's use 'z' for Undo, 'r' for Redo for dev testing.
        if (event.key.keysym.sym == SDLK_z) {
            lines = history.undo(lines);
            return;
        }
        if (event.key.keysym.sym == SDLK_r) {
            lines = history.redo(lines);
            return;
        }

        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: // Back
                app.changeState(std::make_shared<BrowserState>());
                return;
            case SDLK_y: // Cycle Bullet
                if (!lines.empty()) {
                    char current = lines[currentLineIndex].bulletType;
                    auto it = std::find(bullets.begin(), bullets.end(), current);
                    int idx = 0;
                    if (it != bullets.end()) {
                        idx = std::distance(bullets.begin(), it);
                    }
                    idx = (idx + 1) % bullets.size();
                    lines[currentLineIndex].bulletType = bullets[idx];
                    saveHistory();
                }
                return;
            case SDLK_TAB: // Button X simulation (Done)
                 if (!lines.empty()) {
                    Line& l = lines[currentLineIndex];
                    l.completed = !l.completed;
                    if (l.completed) {
                        l.popScale = 1.3f; // Trigger Pop (Distinct 1.3x)
                        l.opacity = 0.5f;
                    } else {
                        l.popScale = 1.0f;
                        l.opacity = 1.0f;
                    }
                    saveHistory();
                 }
                 break;
        }
    }

    bool handled = inputEngine->handleEvent(event);
    bool contentChanged = false;
    
    while (inputEngine->hasInput()) {
        std::string s = inputEngine->popInput();
        
        // Auto-capitalization (Smart Engine)
        if (lines[currentLineIndex].content.empty() && !s.empty()) {
             if (s[0] >= 'a' && s[0] <= 'z') s[0] -= 32;
        }
        
        lines[currentLineIndex].content += s;
        contentChanged = true;
    }

    // New Line Logic (START adds space, let's say SELECT+START is newline or just a dedicated key)
    // For now, let's just use a special key for New Line in dev (e.g., 'n')
    // Or if Return wasn't consumed.
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN && !handled) {
        // New Line
        lines.insert(lines.begin() + currentLineIndex + 1, Line{""});
        currentLineIndex++;
        contentChanged = true;
    }
    
    // Navigation between lines
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
        if (currentLineIndex > 0) currentLineIndex--;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN) {
        if (currentLineIndex < lines.size() - 1) currentLineIndex++;
    }

    if (contentChanged) {
        saveHistory();
    }
}

void EditorState::update(App& app) {
    inputEngine->update();

    // Lerp Progress Bar
    float targetProgress = 0.0f;
    if (!lines.empty()) {
        int completed = 0;
        for (const auto& l : lines) if (l.completed) completed++;
        targetProgress = (float)completed / lines.size();
    }
    
    float lerp = app.getSettings().lerpStrength;
    currentProgress += (targetProgress - currentProgress) * lerp;

    for (auto& line : lines) {
        if (line.popScale > 1.0f) {
            line.popScale -= 0.05f; // Faster decay for snappier feel
            if (line.popScale < 1.0f) line.popScale = 1.0f;
        }
    }
}

void EditorState::render(App& app, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    renderLayout(app, renderer);
    renderProgressBar(app, renderer);

    inputEngine->render(renderer);
}

void EditorState::renderLayout(App& app, SDL_Renderer* renderer) {
    int startY = 60;
    int screenW = app.getScreenWidth();
    int screenH = app.getScreenHeight();

    if (currentLayout == Layout::RAPID_LOG) {
        renderLines(app, renderer, 20, startY, screenW - 40);
    } else if (currentLayout == Layout::CORNELL) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        int splitX = screenW * 0.3;
        int splitY = screenH * 0.8;
        SDL_RenderDrawLine(renderer, splitX, startY, splitX, splitY);
        SDL_RenderDrawLine(renderer, 0, splitY, screenW, splitY);
        // Ensure text respects the 20px safe zone on the right
        renderLines(app, renderer, splitX + 10, startY, screenW - splitX - 30); // 10px padding + 20px safe zone
    } else if (currentLayout == Layout::CHARTING) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        int colW = screenW / 3;
        SDL_RenderDrawLine(renderer, colW, startY, colW, screenH - 100);
        SDL_RenderDrawLine(renderer, colW*2, startY, colW*2, screenH - 100);
        // Safe zone on left for first col, right for last col?
        // Actually renderLines just takes startX and width.
        // Let's just be safe generally.
        renderLines(app, renderer, 20, startY, colW - 30); // Left col safe zone
    }
}

void EditorState::renderLines(App& app, SDL_Renderer* renderer, int startX, int startY, int width) {
    TTF_Font* font = app.getFont();
    int lineHeight = 30;
    
    for (size_t i = 0; i < lines.size(); ++i) {
        int y = startY + (i * lineHeight);
        if (y > 380) break;

        std::string b(1, lines[i].bulletType);
        SDL_Color col = {255, 255, 255, static_cast<Uint8>(255 * lines[i].opacity)};
        if (i == currentLineIndex) col = {255, 200, 100, 255}; 
        
        float scale = lines[i].popScale;
        renderText(renderer, font, b, startX, y, col, scale);
        renderText(renderer, font, lines[i].content, startX + 30, y, col);

        if (lines[i].completed) {
            int w, h;
            TTF_SizeText(font, lines[i].content.c_str(), &w, &h);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
            SDL_RenderDrawLine(renderer, startX + 30, y + h/2, startX + 30 + w, y + h/2);
        }
    }
}

void EditorState::renderProgressBar(App& app, SDL_Renderer* renderer) {
    if (lines.empty()) return;
    
    // Use Safe Zone: 20px margin
    int margin = 20;
    int barW = app.getScreenWidth() - (margin * 2);
    int barX = margin;
    
    SDL_Rect bg = {barX, 10, barW, 10};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &bg);
    
    // Use smoothed currentProgress
    SDL_Rect fill = {barX, 10, static_cast<int>(barW * currentProgress), 10};
    
    if (currentProgress >= 0.99f) {
        Uint8 alpha = 150 + static_cast<int>(100 * sin(SDL_GetTicks() / 200.0f));
        SDL_SetRenderDrawColor(renderer, 100, 255, 100, alpha); // Pulsing Glow
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    }
    SDL_RenderFillRect(renderer, &fill);
}

void EditorState::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color, float scale) {
    if (text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w = surface->w * scale;
    int h = surface->h * scale;
    int offX = (w - surface->w) / 2;
    int offY = (h - surface->h) / 2;
    SDL_Rect dst = {x - offX, y - offY, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

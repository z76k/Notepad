#pragma once
#include "../State.hpp"
#include "../InputEngine.hpp"
#include "../Utils/HistoryManager.hpp"
#include <vector>
#include <string>

struct Line {
    std::string content;
    char bulletType = '*'; // •, O, —, !, ?
    bool completed = false;
    float popScale = 1.0f; // For "Pop" animation
    float opacity = 1.0f;

    bool operator==(const Line& other) const {
        return content == other.content && 
               bulletType == other.bulletType && 
               completed == other.completed;
    }
};

class EditorState : public State {
public:
    EditorState(const std::string& filename = "");
    void enter(App& app) override;
    void exit(App& app) override;
    void handleEvent(App& app, const SDL_Event& event) override;
    void update(App& app) override;
    void render(App& app, SDL_Renderer* renderer) override;

private:
    std::shared_ptr<InputEngine> inputEngine;
    std::string currentFilename;
    
    // Layouts
    enum class Layout {
        RAPID_LOG, // Daily Log
        CORNELL,
        CHARTING
    };
    Layout currentLayout = Layout::RAPID_LOG;

    // Content
    std::vector<Line> lines;
    int currentLineIndex = 0;

    // History
    HistoryManager<std::vector<Line>> history;
    void saveHistory();

    // Satisfaction System
    std::vector<char> bullets = {'*', 'O', '-', '!', '?'}; 
    float currentProgress = 0.0f; // For smooth Lerp animation of progress bar
    
    // Helpers
    void renderLayout(App& app, SDL_Renderer* renderer);
    void renderLines(App& app, SDL_Renderer* renderer, int startX, int startY, int width);
    void renderProgressBar(App& app, SDL_Renderer* renderer);
    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color, float scale = 1.0f);
};

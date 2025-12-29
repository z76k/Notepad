#pragma once
#include "../State.hpp"
#include "../App.hpp"
#include "../Utils/FileSystem.hpp"
#include <vector>
#include <string>

class BrowserState : public State {
public:
    void enter(App& app) override;
    void exit(App& app) override;
    void handleEvent(App& app, const SDL_Event& event) override;
    void update(App& app) override;
    void render(App& app, SDL_Renderer* renderer) override;

private:
    std::vector<std::string> fileList;
    int selectedIndex = 0;
    
    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);
};

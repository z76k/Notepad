#pragma once
#include "../State.hpp"

class DecoyState : public State {
public:
    DecoyState(int mode = 0);
    void enter(App& app) override;
    void exit(App& app) override;
    void handleEvent(App& app, const SDL_Event& event) override;
    void update(App& app) override;
    void render(App& app, SDL_Renderer* renderer) override;

private:
    int mode;
    float scrollOffset = 0.0f;
    std::vector<std::string> logLines;
};

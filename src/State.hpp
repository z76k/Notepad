#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

// Forward declaration
class App;

class State {
public:
    virtual ~State() = default;

    virtual void enter(App& app) = 0;
    virtual void exit(App& app) = 0;
    virtual void handleEvent(App& app, const SDL_Event& event) = 0;
    virtual void update(App& app) = 0;
    virtual void render(App& app, SDL_Renderer* renderer) = 0;
};

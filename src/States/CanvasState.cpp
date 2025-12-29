#include "CanvasState.hpp"
#include "../App.hpp"
#include "BrowserState.hpp"
#include <cmath>
#include <iostream>

void CanvasState::enter(App& app) {
    // Start with one shape if empty
    if (shapes.empty()) {
        addShape(ShapeType::SQUARE, 320, 240);
        selectedShapeIndex = 0;
    }
    saveHistory();
}

void CanvasState::exit(App& app) {
}

CanvasSnapshot CanvasState::createSnapshot() const {
    return {shapes, arrows, nextId};
}

void CanvasState::loadSnapshot(const CanvasSnapshot& snapshot) {
    shapes = snapshot.shapes;
    arrows = snapshot.arrows;
    nextId = snapshot.nextId;
    // Reset selection if out of bounds
    if (selectedShapeIndex >= shapes.size()) {
        selectedShapeIndex = shapes.empty() ? -1 : 0;
    }
}

void CanvasState::saveHistory() {
    history.push(createSnapshot());
}

void CanvasState::addShape(ShapeType type, float x, float y) {
    Shape s;
    s.type = type;
    s.x = x;
    s.y = y;
    s.w = 50;
    s.h = 50;
    if (type == ShapeType::LINE) {
        s.w = x + 50; // End X
        s.h = y;      // End Y
    }
    s.id = nextId++;
    shapes.push_back(s);
}

void CanvasState::handleEvent(App& app, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        bool changed = false;
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                app.changeState(std::make_shared<BrowserState>());
                break;
            
            // Undo/Redo
            case SDLK_z:
                loadSnapshot(history.undo(createSnapshot()));
                break;
            case SDLK_r:
                loadSnapshot(history.redo(createSnapshot()));
                break;

            // Mode Switching / Adding Shapes
            case SDLK_1: addShape(ShapeType::SQUARE, 320, 240); changed = true; break;
            case SDLK_2: addShape(ShapeType::CIRCLE, 320, 240); changed = true; break;
            case SDLK_3: addShape(ShapeType::TRIANGLE, 320, 240); changed = true; break;
            
            // Selection
            case SDLK_TAB:
                if (!shapes.empty()) {
                    selectedShapeIndex = (selectedShapeIndex + 1) % shapes.size();
                }
                break;

            // Copy/Paste (Button X)
            case SDLK_x: 
                if (selectedShapeIndex >= 0 && selectedShapeIndex < shapes.size()) {
                    if (hasClipboard) {
                        // Paste
                        Shape s = clipboardShape;
                        s.x += 20; s.y += 20;
                        s.id = nextId++;
                        shapes.push_back(s);
                        selectedShapeIndex = shapes.size() - 1;
                        hasClipboard = false;
                        changed = true;
                    } else {
                        // Copy
                        clipboardShape = shapes[selectedShapeIndex];
                        hasClipboard = true;
                    }
                }
                break;
            
            // Sticky Arrows (Button A - mapped to 'c' for now)
            case SDLK_c:
                if (selectedShapeIndex > 0) {
                    arrows.push_back({shapes[0].id, shapes[selectedShapeIndex].id});
                    changed = true;
                }
                break;
        }
        if (changed) saveHistory();
    }
    else if (event.type == SDL_KEYUP) {
        // Save history after movement/scaling ends
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT || key == SDLK_UP || key == SDLK_DOWN) {
            saveHistory();
        }
    }

    // Movement & Scaling
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (selectedShapeIndex >= 0 && selectedShapeIndex < shapes.size()) {
        Shape& s = shapes[selectedShapeIndex];
        float speed = 2.0f;
        
        // L1 (q) Modifier for Scaling
        if (state[SDL_SCANCODE_Q]) {
            if (state[SDL_SCANCODE_LEFT]) s.w -= speed;
            if (state[SDL_SCANCODE_RIGHT]) s.w += speed;
            if (state[SDL_SCANCODE_UP]) s.h -= speed;
            if (state[SDL_SCANCODE_DOWN]) s.h += speed;
        } else {
            // Movement
            if (state[SDL_SCANCODE_LEFT]) s.x -= speed;
            if (state[SDL_SCANCODE_RIGHT]) s.x += speed;
            if (state[SDL_SCANCODE_UP]) s.y -= speed;
            if (state[SDL_SCANCODE_DOWN]) s.y += speed;
        }
    }
}


void CanvasState::update(App& app) {
    // Physics or Animations
}

void CanvasState::render(App& app, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    // Render Arrows first (behind shapes)
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    for (const auto& arrow : arrows) {
        // Find shapes
        const Shape* s1 = nullptr;
        const Shape* s2 = nullptr;
        for (const auto& s : shapes) {
            if (s.id == arrow.startShapeId) s1 = &s;
            if (s.id == arrow.endShapeId) s2 = &s;
        }
        if (s1 && s2) {
            SDL_RenderDrawLine(renderer, s1->x, s1->y, s2->x, s2->y);
        }
    }

    for (size_t i = 0; i < shapes.size(); ++i) {
        if (i == selectedShapeIndex) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Highlight
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        renderShape(renderer, shapes[i]);
    }
}

void CanvasState::renderShape(SDL_Renderer* renderer, const Shape& s) {
    SDL_Rect rect = {
        static_cast<int>(s.x - s.w/2), 
        static_cast<int>(s.y - s.h/2), 
        static_cast<int>(s.w), 
        static_cast<int>(s.h)
    };

    switch (s.type) {
        case ShapeType::SQUARE:
            SDL_RenderDrawRect(renderer, &rect);
            break;
        case ShapeType::CIRCLE:
            // Simple circle approximation or use SDL2_gfx if available (not standard)
            // Just draw a rect for now with rounded corners logic or diamond
            // Let's draw a diamond
            {
                SDL_Point points[5] = {
                    {static_cast<int>(s.x), static_cast<int>(s.y - s.h/2)},
                    {static_cast<int>(s.x + s.w/2), static_cast<int>(s.y)},
                    {static_cast<int>(s.x), static_cast<int>(s.y + s.h/2)},
                    {static_cast<int>(s.x - s.w/2), static_cast<int>(s.y)},
                    {static_cast<int>(s.x), static_cast<int>(s.y - s.h/2)}
                };
                SDL_RenderDrawLines(renderer, points, 5);
            }
            break;
        case ShapeType::TRIANGLE:
             {
                SDL_Point points[4] = {
                    {static_cast<int>(s.x), static_cast<int>(s.y - s.h/2)},
                    {static_cast<int>(s.x + s.w/2), static_cast<int>(s.y + s.h/2)},
                    {static_cast<int>(s.x - s.w/2), static_cast<int>(s.y + s.h/2)},
                    {static_cast<int>(s.x), static_cast<int>(s.y - s.h/2)}
                };
                SDL_RenderDrawLines(renderer, points, 4);
            }
            break;
        case ShapeType::LINE:
            SDL_RenderDrawLine(renderer, s.x, s.y, s.w, s.h); // w,h are end points here
            break;
        default: break;
    }
}

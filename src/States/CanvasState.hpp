#pragma once
#include "../State.hpp"
#include <vector>
#include <string>

enum class ShapeType {
    SQUARE,
    CIRCLE,
    TRIANGLE,
    LINE,
    TEXT_NODE
};

struct Shape {
    ShapeType type;
    float x, y;
    float w, h; 
    SDL_Color color = {255, 255, 255, 255};
    std::string text; // For TextNode
    int id; // For linking

    bool operator==(const Shape& other) const {
        return type == other.type &&
               std::abs(x - other.x) < 0.001f &&
               std::abs(y - other.y) < 0.001f &&
               std::abs(w - other.w) < 0.001f &&
               std::abs(h - other.h) < 0.001f &&
               color.r == other.color.r &&
               color.g == other.color.g &&
               color.b == other.color.b &&
               color.a == other.color.a &&
               text == other.text &&
               id == other.id;
    }
};

struct Arrow {
    int startShapeId;
    int endShapeId;

    bool operator==(const Arrow& other) const {
        return startShapeId == other.startShapeId && endShapeId == other.endShapeId;
    }
};

struct CanvasSnapshot {
    std::vector<Shape> shapes;
    std::vector<Arrow> arrows;
    int nextId;

    bool operator==(const CanvasSnapshot& other) const {
        return shapes == other.shapes && arrows == other.arrows && nextId == other.nextId;
    }
};

#include "../Utils/HistoryManager.hpp"

class CanvasState : public State {
public:
    void enter(App& app) override;
    void exit(App& app) override;
    void handleEvent(App& app, const SDL_Event& event) override;
    void update(App& app) override;
    void render(App& app, SDL_Renderer* renderer) override;

private:
    std::vector<Shape> shapes;
    std::vector<Arrow> arrows;
    int selectedShapeIndex = -1;
    int nextId = 0;

    // History
    HistoryManager<CanvasSnapshot> history;
    void saveHistory();
    void loadSnapshot(const CanvasSnapshot& snapshot);
    CanvasSnapshot createSnapshot() const;

    // Clipboard
    Shape clipboardShape;
    bool hasClipboard = false;

    // Mode
    bool isDragging = false;
    
    // Helpers
    void addShape(ShapeType type, float x, float y);
    void renderShape(SDL_Renderer* renderer, const Shape& shape);
    void renderArrow(SDL_Renderer* renderer, const Arrow& arrow);
};

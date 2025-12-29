#pragma once
#include <vector>
#include <deque>

template <typename T>
class HistoryManager {
public:
    void push(const T& state) {
        if (!undoStack.empty() && undoStack.back() == state) return; // Deduplicate
        undoStack.push_back(state);
        if (undoStack.size() > maxHistory) {
            undoStack.pop_front();
        }
        redoStack.clear();
    }

    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

    T undo(const T& currentState) {
        if (undoStack.empty()) return currentState;
        
        // If the top of the stack matches current state, pop it to get to the previous state
        if (undoStack.back() == currentState) {
            undoStack.pop_back();
            if (undoStack.empty()) {
                redoStack.push_back(currentState);
                return currentState;
            }
        }

        redoStack.push_back(currentState);
        T state = undoStack.back();
        undoStack.pop_back();
        return state;
    }

    T redo(const T& currentState) {
        if (redoStack.empty()) return currentState;
        undoStack.push_back(currentState);
        T state = redoStack.back();
        redoStack.pop_back();
        return state;
    }

private:
    std::deque<T> undoStack;
    std::vector<T> redoStack;
    const size_t maxHistory = 100;
};

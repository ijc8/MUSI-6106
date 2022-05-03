#pragma once

#include <filesystem>
#include <future>

#include <juce_gui_extra/juce_gui_extra.h>

#include "BoardComponent.h"
#include "EngineBridge.h"
#include "GameState.h"

class EngineManager: public juce::ChangeListener {
public:
    EngineManager(const std::string &path)
        : engine(path) {
    }

    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
        Chess::Game &game = AppState::getInstance().getGame();
        if (game.getTurn() == Chess::Color::Black) {
            // Wait for engine to pick a move in a separate thread,
            // to avoid blocking the UI.
            task = std::async(std::launch::async, [this, &game] {
                if (onMove) onMove(engine.analyze(game).bestMove);
            });
        }
    }

    std::function<void (Chess::Move)> onMove;

private:
    std::future<void> task;
    Chess::Engine engine;
};

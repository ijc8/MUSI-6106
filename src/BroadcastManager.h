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

class BroadcastManager: public juce::ChangeBroadcaster {
public:
    BroadcastManager(){};
    ~BroadcastManager() { removeAllChangeListeners(); };

    void toggleStockfish(bool shouldTurnOn) {
        if (shouldTurnOn) {
            if (std::filesystem::exists("../../stockfish/stockfish_14.1_win_x64_avx2.exe")) {
                engineManager = std::make_unique<EngineManager>("../../stockfish/stockfish_14.1_win_x64_avx2.exe");
                addChangeListener(engineManager.get());
                engineManager->onMove = [this](Chess::Move move) { makeMove(move); };
            } else {
                // Allow user to tell us where their engine binary is.
                // TODO: Remember their selected engine and allow them to change it later.
                engineChooser = std::make_unique<juce::FileChooser>("Please select the engine executable you want to use...",
                                                                    juce::File::getSpecialLocation(juce::File::userHomeDirectory));

                auto chooserFlags = juce::FileBrowserComponent::openMode;

                engineChooser->launchAsync(chooserFlags, [this](const juce::FileChooser &chooser) {
                    juce::File file = chooser.getResult();
                    if (file.exists()) {
                        engineManager = std::make_unique<EngineManager>(file.getFullPathName().toStdString());
                        addChangeListener(engineManager.get());
                        engineManager->onMove = [this](Chess::Move move) { makeMove(move); };
                    }
                });
            }
        } else {
            if (engineManager)
                removeChangeListener(engineManager.get());
            engineManager.reset();
        }
    }

    void makeMove(Chess::Move move) {
        clearRedoStack();
        game.push(move);
        sendChangeMessage();
    }

    void undo() {
        std::optional<Chess::Move> move = game.pop();
        if (move) {
            redoStack.push(*move);
            sendChangeMessage();
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            Chess::Move lastMove = redoStack.top();
            game.push(lastMove);
            redoStack.pop();
            sendChangeMessage();
        }
    }

    void clearRedoStack() {
        std::stack<Chess::Move> empty;
        redoStack.swap(empty);
    }

private:
    std::unique_ptr<EngineManager> engineManager;
    std::unique_ptr<juce::FileChooser> engineChooser;
    std::stack<Chess::Move> redoStack;
    Chess::Game &game = AppState::getInstance().getGame();
};

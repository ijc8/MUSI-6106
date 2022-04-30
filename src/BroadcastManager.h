#pragma once

#include <filesystem>
#include <future>

#include <juce_gui_extra/juce_gui_extra.h>

#include "GameState.h"
#include "DebugSonifier.h"
#include "BoardComponent.h"
#include "EngineBridge.h"

class EngineManager: public juce::ActionBroadcaster, public juce::ChangeListener {
public:
    EngineManager(const std::string &path)
    : engine(path) {
    }

    ~EngineManager() { removeAllActionListeners(); };

    void changeListenerCallback(juce::ChangeBroadcaster* source) override {
        Game &game = AppState::getInstance().getGame();
        if (game.getTurn() == Color::Black) {
            // Wait for engine to pick a move in a separate thread,
            // to avoid blocking the UI.
            task = std::async(std::launch::async, [this, game]{
                sendActionMessage(engine.analyze(game).bestMove.toString());
            });
        }
    }

private:
    std::future<void> task;
    Chess::Engine engine;
};

class BroadcastManager : public juce::ActionListener, public juce::ChangeBroadcaster
{

public:
   BroadcastManager() {};
  ~BroadcastManager() { removeAllChangeListeners(); };

    void toggleStockfish(bool shouldTurnOn)
    {
        if (shouldTurnOn)
        {
            if (std::filesystem::exists("../../stockfish/stockfish_14.1_win_x64_avx2.exe")) {
                engineManager = std::make_unique<EngineManager>("../../stockfish/stockfish_14.1_win_x64_avx2.exe");
                addChangeListener(engineManager.get());
                engineManager->addActionListener(this);
            } else {
                // Allow user to tell us where their engine binary is.
                // TODO: Remember their selected engine and allow them to change it later.
                engineChooser = std::make_unique<juce::FileChooser>("Please select the engine executable you want to use...",
                    juce::File::getSpecialLocation(juce::File::userHomeDirectory));

                auto chooserFlags = juce::FileBrowserComponent::openMode;

                engineChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser) {
                    juce::File file = chooser.getResult();
                    if (file.exists()) {
                        engineManager = std::make_unique<EngineManager>(file.getFullPathName().toStdString());
                        addChangeListener(engineManager.get());
                        engineManager->addActionListener(this);
                    }
                });
            }
        }
        else
        {
            engineManager.reset();
        }
    }

    void actionListenerCallback(const juce::String& message)
    {
            Chess::Move move = Chess::Move(message.toStdString());
            if (m_Game.isLegal(move))
            {
                emptyUndoHistory();

                m_Game.push(move);
                sendChangeMessage();
            }
    }

    void undo()
    {
        std::optional<Move> move = m_Game.pop();
        if (move) {
            mUndoHistory.push(*move);
            sendChangeMessage();
        }
    }

    void redo()
    {
        if (!mUndoHistory.empty())
        {
            Move lastMove = mUndoHistory.top();
            m_Game.push(lastMove);
            mUndoHistory.pop();
            sendChangeMessage();
        }
    }

    void emptyUndoHistory()
    {
        while (!mUndoHistory.empty())
            mUndoHistory.pop();
    }

private:

    std::unique_ptr<EngineManager> engineManager;
    std::unique_ptr<juce::FileChooser> engineChooser;

    Chess::Game& m_Game = AppState::getInstance().getGame();
    std::stack<Chess::Move> mUndoHistory;

};

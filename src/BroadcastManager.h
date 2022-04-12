#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "GameState.h"
#include "DebugSonifier.h"
#include "ChessboardGUI.h"
#include "EngineBridge.h"

class BroadcastManager : public juce::ActionListener, public juce::ChangeBroadcaster, public juce::ActionBroadcaster
{

public:

    BroadcastManager() : m_Stockfish("../../stockfish/stockfish_14.1_win_x64_avx2.exe") {};

    void toggleStockfish(bool shouldTurnOn)
    {
        m_bStockfishOn = shouldTurnOn;
    }

    void actionListenerCallback(const juce::String& message)
    {
        if (message.contains("Select") || message.contains("Deselect"))
        {
            sendActionMessage(message);
        }
        else 
        {
            Chess::Move move = Chess::Move(Chess::Square(message.substring(0, 2).toStdString()), Chess::Square(message.substring(2, 4).toStdString()));
            if (m_Game.isLegal(move))
            {
                emptyUndoHistory();

                juce::Logger::outputDebugString("Legal Move");
                m_Game.push(move);
                sendChangeMessage();

                if (m_bStockfishOn)
                {
                    m_Game.push(m_Stockfish.analyze(m_Game).bestMove);
                    sendChangeMessage();
                }

            }
            else
                juce::Logger::outputDebugString("Illegal Move");
        }

    }

    void undo()
    {
        if (!m_Game.hasNoHistory())
        {
            mUndoHistory.push(m_Game.pop());
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

    bool m_bStockfishOn = false;
    Stockfish m_Stockfish;
    Chess::Game& m_Game = AppState::getInstance().getGame();
    std::stack<Chess::Move> mUndoHistory;

};

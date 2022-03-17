#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "GameState.h"
#include "DebugSonifier.h"
#include "ChessboardGUI.h"
#include "EngineBridge.h"

class BroadcastManager : public juce::ActionListener, public juce::ChangeBroadcaster
{

public:

    BroadcastManager() : m_Stockfish("../stockfish/stockfish_14.1_win_x64_avx2.exe") {};

    void toggleStockfish(bool shouldTurnOn)
    {
        m_bStockfishOn = shouldTurnOn;
    }

    void actionListenerCallback(const juce::String& message)
    {
        Chess::Move move = Chess::Move(Chess::Square(message.substring(0, 2).toStdString()), Chess::Square(message.substring(2, 4).toStdString()));
        if (m_Game.isLegal(move))
        {
            juce::Logger::outputDebugString("Legal Move");
            m_Game.push(move);
            sendChangeMessage();

            if (m_bStockfishOn)
            {
                m_Stockfish.setState(m_Game);
                m_Game.push(m_Stockfish.getMove());
                sendChangeMessage();
            }

        }
        else
            juce::Logger::outputDebugString("Illegal Move");
    }

private:

    bool m_bStockfishOn = false;
    Stockfish m_Stockfish;
    Chess::Game& m_Game = AppState::getInstance().getGame();

};
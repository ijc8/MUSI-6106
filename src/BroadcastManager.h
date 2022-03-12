#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "GameState.h"
#include "DebugSonifier.h"
#include "ChessboardGUI.h"

class BroadcastManager : public juce::ActionListener, public juce::ChangeBroadcaster
{
public:

    void actionListenerCallback(const juce::String& message)
    {
        Chess::Move move = Chess::Move(Chess::Square(message.substring(0, 2).toStdString()), Chess::Square(message.substring(2, 4).toStdString()));
        if (m_Game.isLegal(move))
        {
            juce::Logger::outputDebugString("Legal Move");
            m_Game.push(move);
            sendChangeMessage();
        }
        else
            juce::Logger::outputDebugString("Illegal Move");
    }

private:

    Chess::Game& m_Game = AppState::getInstance().getGame();

};
#pragma once

#include "ChessImageData.h"
#include "GameState.h"
#include <juce_gui_extra/juce_gui_extra.h>

class BoardComponent: public juce::Component,
                      public juce::ActionBroadcaster,
                      public juce::ChangeListener,
                      public juce::ActionListener {
  public:
    enum class State {
        Idle,
        Placing,
        Switching,
    };

    enum class Mode {
        PVP,
        PVC,
        PGN,
    };

    BoardComponent();
    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void actionListenerCallback(const juce::String &message) override;
    void onModeChange(Mode newMode);

  private:
    static constexpr int BoardSize = 8;
    static constexpr int NumPieces = 32;

    std::unordered_map<Chess::Piece, juce::Image> pieceImages;

    Mode m_CurrentMode = Mode::PVP;
    State m_CurrentState = State::Idle;
    std::optional<Chess::Square> selected;
    struct FloatingPiece { Chess::Piece piece; juce::Point<float> offset; };
    std::optional<FloatingPiece> dragging;

    float getSquareSize() const;
    Chess::Square coordsToSquare(int x, int y) const;
    juce::Rectangle<float> squareToRect(Chess::Square square) const;
    void makeMove(const juce::MouseEvent &event);
    void onStateChange(State newState);
};

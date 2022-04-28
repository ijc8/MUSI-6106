#pragma once

#include "ChessImageData.h"
#include "GameState.h"
#include <juce_gui_extra/juce_gui_extra.h>

class BoardComponent: public juce::Component, public juce::ActionBroadcaster, public juce::ChangeListener {
  public:
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
    void setMode(Mode newMode);

  private:
    std::unordered_map<Chess::Piece, juce::Image> pieceImages;

    Mode mode = Mode::PVP;
    std::optional<Chess::Square> selected;

    struct FloatingPiece {
        Chess::Piece piece;
        juce::Point<float> offset;
    };
    std::optional<FloatingPiece> dragging;

    float getSquareSize() const;
    Chess::Square coordsToSquare(int x, int y) const;
    juce::Rectangle<float> squareToRect(Chess::Square square) const;
    void makeMove(const juce::MouseEvent &event);
    void select(std::optional<Chess::Square> square = std::nullopt);
};

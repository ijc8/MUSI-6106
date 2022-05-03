#pragma once

#include "ChessImageData.h"
#include "GameState.h"
#include <juce_gui_extra/juce_gui_extra.h>

class BoardComponent: public juce::Component {
public:
    BoardComponent();
    void select(std::optional<Chess::Square> square = std::nullopt);
    void enableInput(Chess::Color color, bool enable);

    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

    std::function<void(Chess::Move)> onMove;

private:
    std::unordered_map<Chess::Piece, juce::Image> pieceImages;

    // Whether local input is allowed for each player (black & white).
    bool inputEnabled[2] = {true, true};
    std::optional<Chess::Square> selected;

    struct FloatingPiece {
        Chess::Piece piece;
        juce::Point<float> offset;
    };
    std::optional<FloatingPiece> dragging;

    Chess::Square coordsToSquare(int x, int y) const;
    juce::Rectangle<float> squareToRect(Chess::Square square) const;
    void makeMove(const juce::MouseEvent &event);
};

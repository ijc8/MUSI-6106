#include "BoardComponent.h"

BoardComponent::BoardComponent() {
    std::pair<Chess::Color, std::string> colors[] = {{Chess::Color::White, "w"}, {Chess::Color::Black, "b"}};
    // Load chess piece images.
    for (auto &[type, typeName] : Chess::Piece::ToChar) {
        for (auto &[color, colorName] : colors) {
            int size;
            std::string resourceName = colorName + typeName + "_png";
            const char *data = ChessImageData::getNamedResource(resourceName.c_str(), size);
            pieceImages.emplace(Chess::Piece(type, color), juce::ImageFileFormat::loadFrom(data, size));
        }
    }
}

void BoardComponent::paint(juce::Graphics &g) {
    static const juce::Colour lightSquare(240, 217, 181);
    static const juce::Colour darkSquare(181, 136, 99);

    // Draw squares.
    // NOTE: We assume our parent is maintaining our 1:1 aspect ratio.
    int size = getWidth();
    float squareSize = getSquareSize();
    g.fillCheckerBoard(juce::Rectangle<float>(0, 0, size, size), squareSize, squareSize, lightSquare, darkSquare);

    if (selected) {
        g.setColour(juce::Colours::red.withAlpha(0.5f));
        g.fillRect(squareToRect(*selected));
        // TODO: Maybe cache this.
        auto candidates = AppState::getInstance().getGame().generateMoves(*selected);
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        for (auto move : candidates) {
            g.fillRect(squareToRect(move.dst));
        }
        g.setColour(juce::Colours::black);
    }

    // Draw pieces.
    for (auto [square, piece] : AppState::getInstance().getGame().getPieceMap()) {
        float x = squareSize * square.file;
        float y = squareSize * (7 - square.rank);
        juce::Image &image = pieceImages[piece];
        g.drawImage(image, juce::Rectangle<float>(x, y, squareSize, squareSize));
    }

    if (dragging) {
        juce::Image &image = pieceImages[dragging->piece];
        juce::Point<int> mousePos = getMouseXYRelative();
        float x = mousePos.x - dragging->offset.x;
        float y = mousePos.y - dragging->offset.y;
        g.drawImage(image, juce::Rectangle<float>(x, y, squareSize, squareSize));
    }
}

float BoardComponent::getSquareSize() const {
    return (float)getWidth() / 8;
}

Chess::Square BoardComponent::coordsToSquare(int x, int y) const {
    float squareSize = getSquareSize();
    return Chess::Square(7 - (int)(y / squareSize), (int)(x / squareSize));
}

juce::Rectangle<float> BoardComponent::squareToRect(Chess::Square square) const {
    float squareSize = getSquareSize();
    float x = squareSize * square.file;
    float y = squareSize * (7 - square.rank);
    return juce::Rectangle<float>(x, y, squareSize, squareSize);
}

void BoardComponent::makeMove(const juce::MouseEvent &event) {
    Chess::Game &game = AppState::getInstance().getGame();
    Chess::Square target = coordsToSquare(event.x, event.y);
    Chess::Piece selectedPiece = *game.getPieceAt(*selected);
    Chess::Color turn = game.getTurn();

    // Don't show promotions menu or make move unless it is to a candidate square.
    Chess::Move move(*selected, target);
    std::unordered_set<Chess::Move> moves = game.generateMoves(*selected);
    bool valid = false;
    for (auto candidate : moves) {
        if (move.src == candidate.src && move.dst == candidate.dst) {
            valid = true;
            break;
        }
    }

    if (!valid) {
        selected.reset();
        return;
    }

    if (selectedPiece.type == Chess::Piece::Type::Pawn &&
        target.rank == (turn == Chess::Color::White ? 7 : 0)) {
        // Pawn promotion - need to prompt the user to select which piece they want.
        juce::PopupMenu menu;
        static const std::pair<std::string, Chess::Piece::Type> promotions[] = {
            {"Queen", Chess::Piece::Type::Queen},
            {"Knight", Chess::Piece::Type::Knight},
            {"Rook", Chess::Piece::Type::Rook},
            {"Bishop", Chess::Piece::Type::Bishop},
        };
        for (int i = 0; i < sizeof(promotions) / sizeof(*promotions); i++) {
            auto &[name, type] = promotions[i];
            menu.addItem(i + 1, name, true, false, pieceImages[Chess::Piece(type, turn)]);
        }
        menu.showMenuAsync(juce::PopupMenu::Options(), [this, target](int result) {
            if (result == 0) {
                // User dismissed the promotion menu; cancel the move.
                select();
            } else {
                // Complete move by filling in the promotion.
                Chess::Piece::Type type = promotions[result - 1].second;
                if (onMove) onMove(Chess::Move(*selected, target, type));
                selected.reset();
            }
        });
    } else {
        if (onMove) onMove(move);
        selected.reset();
    }
}

void BoardComponent::mouseDown(const juce::MouseEvent &event) {
    Chess::Square target = coordsToSquare(event.x, event.y);
    juce::Rectangle<float> rect = squareToRect(target);
    Chess::Game &game = AppState::getInstance().getGame();
    Chess::Color turn = game.getTurn();
    std::optional<Chess::Piece> piece = game.getPieceAt(target);

    if (selected) {
        Chess::Piece selectedPiece = *game.getPieceAt(*selected);
        if (target == *selected) {
            select();
        } else if (piece && selectedPiece.color == piece->color) {
            select(target);
            dragging = {*piece, juce::Point<float>(event.x - rect.getX(), event.y - rect.getY())};
        } else {
            makeMove(event);
        }
    } else {
        if (inputEnabled[(int)turn] && piece && turn == piece->color) {
            select(target);
            dragging = {*piece, juce::Point<float>(event.x - rect.getX(), event.y - rect.getY())};
        }
    }
}

void BoardComponent::mouseDrag(const juce::MouseEvent &event) {
    (void)event;
    repaint();
}

void BoardComponent::mouseUp(const juce::MouseEvent &event) {
    if (!dragging) return;
    Chess::Square target = coordsToSquare(event.x, event.y);
    if (target != *selected) {
        makeMove(event);
    }
    dragging.reset();
    repaint();
}

void BoardComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    (void)source;
    select();
}

void BoardComponent::enableInput(Chess::Color color, bool enable) {
    inputEnabled[(int)color] = enable;
    if (!enable && color == AppState::getInstance().getGame().getTurn()) {
        select();
    }
}

void BoardComponent::select(std::optional<Chess::Square> square) {
    selected = square;
    repaint();
}

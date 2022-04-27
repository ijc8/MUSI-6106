#include "BoardComponent.h"

BoardComponent::BoardComponent() {
    std::pair<Chess::Piece::Type, std::string> types[] = {
        {Chess::Piece::Type::Pawn, "Pawn"},
        {Chess::Piece::Type::Knight, "Knight"},
        {Chess::Piece::Type::Bishop, "Bishop"},
        {Chess::Piece::Type::Rook, "Rook"},
        {Chess::Piece::Type::Queen, "Queen"},
        {Chess::Piece::Type::King, "King"},
    };
    std::pair<Chess::Color, std::string> colors[] = {{Chess::Color::White, "W"}, {Chess::Color::Black, "B"}};

    for (auto &[type, typeName] : types) {
        for (auto &[color, colorName] : colors) {
            int size;
            std::string resourceName = colorName + "_" + typeName + "_png";
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
    float pieceHeight = squareSize * .8;
    float topMargin = (squareSize - pieceHeight) / 2.0;
    for (auto [square, piece] : AppState::getInstance().getGame().getPieceMap()) {
        float x = squareSize * square.file;
        float y = squareSize * (7 - square.rank);
        juce::Image &image = pieceImages[piece];
        float pieceWidth = pieceHeight * image.getWidth() / image.getHeight();
        float leftMargin = (squareSize - pieceWidth) / 2.0;
        g.drawImage(image, juce::Rectangle<float>(x + leftMargin, y + topMargin, pieceWidth, pieceHeight));
    }

    if (dragging) {
        juce::Image &image = pieceImages[dragging->piece];
        float pieceWidth = pieceHeight * image.getWidth() / image.getHeight();
        float leftMargin = (squareSize - pieceWidth) / 2.0;
        juce::Point<int> mousePos = getMouseXYRelative();
        float x = mousePos.x - dragging->offset.x + leftMargin;
        float y = mousePos.y - dragging->offset.y + topMargin;
        g.drawImage(image, juce::Rectangle<float>(x, y, pieceWidth, pieceHeight));
    }
}

float BoardComponent::getSquareSize() const {
    return (float)getWidth() / BoardSize;
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

    if (selectedPiece.type == Chess::Piece::Type::Pawn &&
                target.rank == (turn == Chess::Color::White ? 7 : 0)) {
        // Pawn promotion - need to prompt the user to select which piece they want.
        juce::PopupMenu m;
        static const std::pair<std::string, Chess::Piece::Type> promotions[] = {
            {"Queen", Chess::Piece::Type::Queen},
            {"Knight", Chess::Piece::Type::Knight},
            {"Rook", Chess::Piece::Type::Rook},
            {"Bishop", Chess::Piece::Type::Bishop},
        };
        for (int i = 0; i < sizeof(promotions) / sizeof(*promotions); i++) {
            auto &[name, type] = promotions[i];
            m.addItem(i + 1, name, true, false, pieceImages[Chess::Piece(type, turn)]);
        }
        m.showMenuAsync(juce::PopupMenu::Options(), [this, target](int result) {
            if (result == 0) {
                // User dismissed the promotion menu; cancel the move.
                selected.reset();
                onStateChange(State::Idle);
            } else {
                // Complete move by filling in the promotion.
                Chess::Piece::Type type = promotions[result - 1].second;
                juce::String intent = Chess::Move(*selected, target, type).toString();
                sendActionMessage(intent);
                selected.reset();
            }
        });
    } else {
        juce::String intent = Chess::Move(*selected, target).toString();
        sendActionMessage(intent);
        selected.reset();
    }
}

void BoardComponent::mouseDown(const juce::MouseEvent &event) {
    if (m_CurrentMode == Mode::PGN) return;

    Chess::Square target = coordsToSquare(event.x, event.y);
    juce::Rectangle<float> rect = squareToRect(target);
    Chess::Game &game = AppState::getInstance().getGame();
    Chess::Color turn = game.getTurn();
    std::optional<Chess::Piece> piece = game.getPieceAt(target);
    Chess::Piece selectedPiece = *game.getPieceAt(*selected);

    if (selected) {
        if (target == *selected) {
            onStateChange(State::Idle);
            selected.reset();
        } else if (piece && selectedPiece.color == piece->color) {
            onStateChange(State::Switching);
            // selectPiece(piece);
            selected = target;
            dragging = {*piece, juce::Point<float>(event.x - rect.getX(), event.y - rect.getY())};
        } else {
            makeMove(event);
        }
    } else {
        if (m_CurrentMode == Mode::PVP) {
            if (piece && turn == piece->color && (m_CurrentMode == Mode::PVP || turn == Chess::Color::White)) {
                // selectPiece(piece);
                onStateChange(State::Placing);
                selected = target;
                dragging = {*piece, juce::Point<float>(event.x - rect.getX(), event.y - rect.getY())};
            }
        }
    }

    // sendActionMessage("Preview " + getId()) ?
}

void BoardComponent::mouseDrag(const juce::MouseEvent &event) {
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
    onStateChange(State::Idle);
}

void BoardComponent::actionListenerCallback(const juce::String &message) {
    if (message.contains("Preview")) {
        // if (m_SelectedPiece) {
        //     juce::String newMessage = message.substring(0, 8) +
        //                               m_SelectedPiece->getSquareId() +
        //                               message.substring(8, 10);
        //     sendActionMessage(newMessage);
        // }
    }
}

void BoardComponent::onModeChange(BoardComponent::Mode newMode) {
    m_CurrentMode = newMode;
}

// void BoardComponent::selectPiece(Piece &piece) {
//     if (m_SelectedPiece) {
//         if (m_SelectedPiece->getId() != piece.getId())
//             sendActionMessage("Deselect " + m_SelectedPiece->getId());
//     }
//     sendActionMessage("Select " + m_SelectedPiece->getId());
//     onStateChange(State::Placing);
// }

void BoardComponent::onStateChange(BoardComponent::State newState) {
    switch (newState) {
    case State::Idle:
        // TODO: sendActionMessage("Deselect " + m_SelectedPiece->getId());
        break;
    case State::Switching:
        break;
    default:;
    }

    m_CurrentState = newState;
    repaint();
}

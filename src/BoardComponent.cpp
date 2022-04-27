#include "BoardComponent.h"

// void BoardComponent::Square::paintButton(juce::Graphics &g,
//                                          bool shouldDrawButtonAsHighlighted,
//                                          bool shouldDrawButtonAsDown) {
//     if (m_IsCandidate) {
//         if (shouldDrawButtonAsHighlighted) {
//             g.fillAll(juce::Colours::red);
//             sendActionMessage("Preview " + getId());
//         } else
//             g.fillAll(juce::Colours::yellow);
//     } else
//         g.fillAll(m_SquareColor);
// }

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
    float squareSize = (float)size / BoardSize;
    g.fillCheckerBoard(juce::Rectangle<float>(0, 0, size, size), squareSize, squareSize, lightSquare, darkSquare);

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
}

void BoardComponent::buttonClicked(juce::Button *button) {
    if (m_CurrentMode != Mode::PGN) {
        // for (Piece &piece : m_AllPieces) {
        //     if (button == &piece) {
        //         if (m_SelectedPiece) {
        //             if (m_SelectedPiece == &piece) {
        //                 onStateChange(State::Idle);
        //             } else if (m_SelectedPiece->isAlly(piece)) {
        //                 onStateChange(State::Switching);
        //                 m_SelectedPiece->setToggleState(
        //                     false, juce::dontSendNotification);
        //                 selectPiece(piece);
        //             } else {
        //                 juce::String intent = m_SelectedPiece->getSquareId() +
        //                                       piece.getSquareId();
        //                 sendActionMessage(intent);
        //             }
        //         } else {
        //             if (m_CurrentMode == Mode::PVP) {
        //                 if (AppState::getInstance().getGame().getTurn() ==
        //                     piece.getTeam()) {
        //                     selectPiece(piece);
        //                     onStateChange(State::Placing);
        //                 }
        //             } else {
        //                 if (AppState::getInstance().getGame().getTurn() ==
        //                         Chess::Color::White &&
        //                     piece.getTeam() == Chess::Color::White) {
        //                     selectPiece(piece);
        //                     onStateChange(State::Placing);
        //                 }
        //             }
        //         }
        //         return;
        //     }
        // }

        // for (int row = 0; row < BoardSize; row++) {
        //     for (int col = 0; col < BoardSize; col++) {
        //         Square *&square = m_AllSquares[row][col];
        //         if (button == square) {
        //             if (m_CurrentState == State::Placing) {
        //                 juce::String intent =
        //                     m_SelectedPiece->getSquareId() + square->getId();
        //                 sendActionMessage(intent);
        //             }
        //             return;
        //         }
        //     }
        // }
    }
}

void BoardComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    // std::list<Piece *> pieceList;
    // for (Piece &piece : m_AllPieces) {
    //     piece.setVisible(false);
    //     pieceList.push_front(&piece);
    // }

    auto pieceMap = AppState::getInstance().getGame().getPieceMap();
    for (const auto [square, piece] : pieceMap) {
        // for (Piece *guiPiece : pieceList) {
        //     if (guiPiece->getId().toStdString() ==
        //         std::string{piece.toChar()}) {
        //         guiPiece->setVisible(true);
        //         // guiPiece->placeAt(findSquare(square.toString()));
        //         pieceList.remove(guiPiece);
        //         break;
        //     }
        // }
    }
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
    switch (newMode) {
    case Mode::PVP:
        break;
    case Mode::PVC:
        break;
    default:;
    }
    m_CurrentMode = newMode;
}

// void BoardComponent::selectPiece(Piece &piece) {
//     if (m_SelectedPiece) {
//         m_SelectedPiece->setSelected(false);
//         if (m_SelectedPiece->getId() != piece.getId())
//             sendActionMessage("Deselect " + m_SelectedPiece->getId());
//     }
//     m_SelectedPiece = &piece;
//     m_SelectedPiece->setSelected(true);
//     m_SelectedPiece->setToggleState(true, juce::dontSendNotification);
//     highlightPossibleMoves(m_SelectedPiece);
//     sendActionMessage("Select " + m_SelectedPiece->getId());
//     onStateChange(State::Placing);
// }

// BoardComponent::Square *
// BoardComponent::findSquare(const std::string &squareId) const {
//     for (int row = 0; row < BoardSize; row++) {
//         for (int col = 0; col < BoardSize; col++) {
//             Square *square = m_AllSquares[row][col];
//             if (squareId == square->getId())
//                 return square;
//         }
//     }
// }

// BoardComponent::Square *BoardComponent::findSquare(const Piece &piece) const {
//     return findSquare(piece.getSquareId());
// }

// void BoardComponent::highlightPossibleMoves(const Piece *piece) {
//     if (piece) {
//         Chess::Game game = AppState::getInstance().getGame();
//         std::unordered_set moves =
//             game.generateMoves(Chess::Square(piece->getSquareId()));
//         for (const Chess::Move &move : moves) {
//             Square *square = findSquare(move.dst.toString());
//             square->isCandidate(true);
//         }
//     }
// }

void BoardComponent::resetPossibleMoves() {
    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            // Square *&square = m_AllSquares[row][col];
            // square->isCandidate(false);
        }
    }
}

void BoardComponent::onStateChange(BoardComponent::State newState) {
    switch (newState) {
    case State::Idle:
        // if (m_SelectedPiece) {
        //     m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
        //     m_SelectedPiece->setSelected(false);
        //     sendActionMessage("Deselect " + m_SelectedPiece->getId());
        // }
        // m_SelectedPiece = nullptr;
        resetPossibleMoves();
        break;
    case State::Switching:
        resetPossibleMoves();
        break;
    default:;
    }

    m_CurrentState = newState;
}

#include "BoardComponent.h"

BoardComponent::Square::Square(int row, int column, juce::Colour color)
    : juce::Button("square"), m_SquareColor(color), m_Row(row), m_Col(column),
      m_IsCandidate(false) {
    setSize(80, 80);
}

void BoardComponent::Square::paintButton(juce::Graphics &g,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool shouldDrawButtonAsDown) {
    if (m_IsCandidate) {
        if (shouldDrawButtonAsHighlighted) {
            g.fillAll(juce::Colours::red);
            sendActionMessage("Preview " + getId());
        } else
            g.fillAll(juce::Colours::yellow);
    } else
        g.fillAll(m_SquareColor);
}

BoardComponent::BoardComponent() {
    auto genColor = [](int row, int col) {
        if (row % 2 == 0) {
            if (col % 2 == 0)
                return juce::Colour(240, 217, 181);
            else
                return juce::Colour(181, 136, 99);
        } else {
            if (col % 2 == 0)
                return juce::Colour(181, 136, 99);
            else
                return juce::Colour(240, 217, 181);
        }
    };

    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            Square *square = new Square(row, col, genColor(row, col));
            addAndMakeVisible(square);
            square->addListener(this);
            square->addActionListener(this);
            m_AllSquares[row][col] = square;
        }
    }

    for (Piece &piece : m_AllPieces) {
        addAndMakeVisible(piece);
        piece.placeAt(findSquare(piece));
        piece.addListener(this);
    }
}

BoardComponent::~BoardComponent() {
    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            Square *&square = m_AllSquares[row][col];
            square->removeListener(this);
            square->removeActionListener(this);
            delete square;
            square = nullptr;
        }
    }

    for (Piece &piece : m_AllPieces)
        piece.removeListener(this);
}

void BoardComponent::resized() {
    auto area = getBounds();
    int squareHeight = area.getHeight() / BoardSize;
    int squareWidth = area.getWidth() / BoardSize;

    int ypos = area.getHeight();
    for (int row = 0; row < BoardSize; row++) {
        ypos -= squareHeight;
        int xpos = 0;
        for (int col = 0; col < BoardSize; col++) {
            m_AllSquares[row][col]->setBounds(xpos, ypos, squareWidth,
                                              squareHeight);
            xpos += squareWidth;
        }
    }

    for (Piece &piece : m_AllPieces) {
        piece.resized();
    }
}

void BoardComponent::buttonClicked(juce::Button *button) {
    if (m_CurrentMode != BoardComponent::mode::kPGN) {
        for (Piece &piece : m_AllPieces) {
            if (button == &piece) {
                if (m_SelectedPiece) {
                    if (m_SelectedPiece == &piece) {
                        onStateChange(state::kIdle);
                    } else if (m_SelectedPiece->isAlly(piece)) {
                        onStateChange(state::kSwitching);
                        m_SelectedPiece->setToggleState(
                            false, juce::dontSendNotification);
                        selectPiece(piece);
                    } else {
                        juce::String intent = m_SelectedPiece->getSquareId() +
                                              piece.getSquareId();
                        sendActionMessage(intent);
                    }
                } else {
                    if (m_CurrentMode == BoardComponent::mode::kPVP) {
                        if (AppState::getInstance().getGame().getTurn() ==
                            piece.getTeam()) {
                            selectPiece(piece);
                            onStateChange(state::kPlacing);
                        }
                    } else {
                        if (AppState::getInstance().getGame().getTurn() ==
                                Chess::Color::White &&
                            piece.getTeam() == Chess::Color::White) {
                            selectPiece(piece);
                            onStateChange(state::kPlacing);
                        }
                    }
                }
                return;
            }
        }

        for (int row = 0; row < BoardSize; row++) {
            for (int col = 0; col < BoardSize; col++) {
                Square *&square = m_AllSquares[row][col];
                if (button == square) {
                    if (m_CurrentState == state::kPlacing) {
                        juce::String intent =
                            m_SelectedPiece->getSquareId() + square->getId();
                        sendActionMessage(intent);
                    }
                    return;
                }
            }
        }
    }
}

void BoardComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    std::list<Piece *> pieceList;
    for (Piece &piece : m_AllPieces) {
        piece.setVisible(false);
        pieceList.push_front(&piece);
    }

    auto pieceMap = AppState::getInstance().getGame().getPieceMap();
    for (const auto [square, piece] : pieceMap) {
        for (Piece *guiPiece : pieceList) {
            if (guiPiece->getId().toStdString() ==
                std::string{piece.toChar()}) {
                guiPiece->setVisible(true);
                guiPiece->placeAt(findSquare(square.toString()));
                pieceList.remove(guiPiece);
                break;
            }
        }
    }
    onStateChange(state::kIdle);
}

void BoardComponent::actionListenerCallback(const juce::String &message) {
    if (message.contains("Preview")) {
        if (m_SelectedPiece) {
            juce::String newMessage = message.substring(0, 8) +
                                      m_SelectedPiece->getSquareId() +
                                      message.substring(8, 10);
            sendActionMessage(newMessage);
        }
    }
}

void BoardComponent::onModeChange(BoardComponent::mode newMode) {
    switch (newMode) {
    case BoardComponent::mode::kPVP:
        break;
    case BoardComponent::mode::kPVC:
        break;
    default:;
    }
    m_CurrentMode = newMode;
}

void BoardComponent::selectPiece(Piece &piece) {
    if (m_SelectedPiece) {
        m_SelectedPiece->setSelected(false);
        if (m_SelectedPiece->getId() != piece.getId())
            sendActionMessage("Deselect " + m_SelectedPiece->getId());
    }
    m_SelectedPiece = &piece;
    m_SelectedPiece->setSelected(true);
    m_SelectedPiece->setToggleState(true, juce::dontSendNotification);
    highlightPossibleMoves(m_SelectedPiece);
    sendActionMessage("Select " + m_SelectedPiece->getId());
    onStateChange(state::kPlacing);
}

BoardComponent::Square *
BoardComponent::findSquare(const std::string &squareId) const {
    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            Square *square = m_AllSquares[row][col];
            if (squareId == square->getId())
                return square;
        }
    }
}

BoardComponent::Square *BoardComponent::findSquare(const Piece &piece) const {
    return findSquare(piece.getSquareId());
}

void BoardComponent::highlightPossibleMoves(const Piece *piece) {
    if (piece) {
        Chess::Game game = AppState::getInstance().getGame();
        std::unordered_set moves =
            game.generateMoves(Chess::Square(piece->getSquareId()));
        for (const Chess::Move &move : moves) {
            Square *square = findSquare(move.dst.toString());
            square->isCandidate(true);
        }
    }
}

void BoardComponent::resetPossibleMoves() {
    for (int row = 0; row < BoardSize; row++) {
        for (int col = 0; col < BoardSize; col++) {
            Square *&square = m_AllSquares[row][col];
            square->isCandidate(false);
        }
    }
}

void BoardComponent::onStateChange(BoardComponent::state newState) {
    switch (newState) {
    case BoardComponent::state::kIdle:
        if (m_SelectedPiece) {
            m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
            m_SelectedPiece->setSelected(false);
            sendActionMessage("Deselect " + m_SelectedPiece->getId());
        }
        m_SelectedPiece = nullptr;
        resetPossibleMoves();
        break;
    case BoardComponent::state::kSwitching:
        resetPossibleMoves();
        break;
    default:;
    }

    m_CurrentState = newState;
}

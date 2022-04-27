#pragma once

#include "ChessImageData.h"
#include "GameState.h"
#include <juce_gui_extra/juce_gui_extra.h>

class BoardComponent: public juce::Component,
                      public juce::Button::Listener,
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

    class Piece: public juce::ImageButton {
      public:
        Piece(const char *resourceName, uint8_t name,
              std::string intialSquareId)
            : m_Name(name), m_SquareId(intialSquareId),
              m_Team(
                  (isupper(name) ? Chess::Color::White : Chess::Color::Black)) {
            int size;
            const char *data =
                ChessImageData::getNamedResource(resourceName, size);
            m_Image = juce::ImageFileFormat::loadFrom(data, size);
            setImages(
                false, true, true, m_Image, 1, juce::Colours::transparentBlack,
                juce::Image(nullptr), 0.5, juce::Colours::transparentWhite,
                juce::Image(nullptr), 0.5, juce::Colours::transparentWhite);
            setSize(80, 80);
        };
        ~Piece() = default;

        juce::String getId() const {
            return juce::String::charToString(m_Name);
        };
        void setSquareId(std::string newId) { m_SquareId = newId; };
        std::string getSquareId() const { return m_SquareId; };
        Chess::Color getTeam() const { return m_Team; };
        bool operator==(const Piece &lhs) const {
            return (this->m_Name == lhs.m_Name &&
                    this->m_SquareId == lhs.m_SquareId);
        }

        bool isAlly(const Piece &piece) const { return m_Team == piece.m_Team; }

        // void placeAt(const Square *square) {
        //     m_Square = square;
        //     m_SquareId = square->getId();
        //     setBounds(square->getBounds());
        // }

        void resized() override {
            // if (m_Square)
            //     setBounds(m_Square->getBounds());
        }

        void setSelected(bool isSelected) { mIsSelected = isSelected; }

        void mouseDown(const juce::MouseEvent &event) override {
            ImageButton::mouseDown(event);
        }

        void mouseDrag(const juce::MouseEvent &event) override {
            if (!mIsSelected) {
                triggerClick();
            }
            juce::MouseEvent relEvent =
                event.getEventRelativeTo(getParentComponent());
            setCentrePosition(relEvent.getPosition());
            mWasBeingDragged = true;
        }

        void mouseUp(const juce::MouseEvent &event) override {
            if (mWasBeingDragged) {
                for (juce::Component *component :
                     getParentComponent()->getChildren()) {
                    if (component != this &&
                        component->getBoundsInParent().contains(
                            event.getEventRelativeTo(getParentComponent())
                                .getPosition())) {
                        component->mouseDown(event);
                        component->mouseUp(event);
                    }
                }
                mWasBeingDragged = false;
            } else {
                ImageButton::mouseUp(event);
            }
            resized();
        }

      private:
        bool mIsSelected = false;
        bool mWasBeingDragged = false;
        // const Square *m_Square = nullptr;
        juce::Image m_Image;
        uint8_t m_Name;
        std::string m_SquareId;
        const Chess::Color m_Team = Chess::Color::White;
    };

    BoardComponent();
    void paint(juce::Graphics &g) override;
    void resized() override;
    void buttonClicked(juce::Button *button) override;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void actionListenerCallback(const juce::String &message) override;
    void onModeChange(Mode newMode);

  private:
    static constexpr int BoardSize = 8;
    static constexpr int NumPieces = 32;

    Mode m_CurrentMode = Mode::PVP;
    State m_CurrentState = State::Idle;
    Piece *m_SelectedPiece = nullptr;
    // Square *m_AllSquares[BoardSize][BoardSize]{nullptr};

    std::unordered_map<Chess::Piece, juce::Image> pieceImages;

    void selectPiece(Piece &piece);
    // Square *findSquare(const std::string &squareId) const;
    // Square *findSquare(const Piece &piece) const;
    void highlightPossibleMoves(const Piece *piece);
    void resetPossibleMoves();
    void onStateChange(State newState);
};

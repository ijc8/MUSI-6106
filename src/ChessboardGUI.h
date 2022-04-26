#pragma once

#include <iostream>
#include <juce_gui_extra/juce_gui_extra.h>
#include "ChessImageData.h"
#include "GameState.h"


namespace GUI
{

	class Square : public juce::Button, public juce::ActionBroadcaster
	{
	public:
		Square(int row, int column, juce::Colour color) : juce::Button("square"), m_SquareColor(color), m_Row(row), m_Col(column), m_IsCandidate(false)
		{
			m_Rank = toRank(row);
			m_File = toFile(column);
			setId(m_Rank, m_File);
			setSize(80, 80);
		};

		~Square() = default;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
		{
			if (m_IsCandidate)
			{
				if (shouldDrawButtonAsHighlighted)
				{
					g.fillAll(juce::Colours::red);
					sendActionMessage("Preview " + getId());
				}
				else
					g.fillAll(juce::Colours::yellow);
			}
			else
				g.fillAll(m_SquareColor);
		}

		void resized() override
		{
			setBounds(getBounds());
		}

		uint8_t getRank() const { return m_Rank; };
		uint8_t getFile() const { return m_File; };
		juce::String getId() const { return m_Id; };

		void isCandidate(bool isCandidate) { m_IsCandidate = isCandidate; repaint(); };

	private:

		uint8_t toRank(int row)
		{
			switch (row)
			{
			case 0: return '1';
			case 1: return '2';
			case 2: return '3';
			case 3: return '4';
			case 4: return '5';
			case 5: return '6';
			case 6: return '7';
			default: return '8';
			}
		}

		uint8_t toFile(int col)
		{
			switch (col)
			{
			case 0: return 'a';
			case 1: return 'b';
			case 2: return 'c';
			case 3: return 'd';
			case 4: return 'e';
			case 5: return 'f';
			case 6: return 'g';
			default: return 'h';
			}
		}

		void setId(uint8_t rank, uint8_t file)
		{
			m_Id = juce::String::charToString(file) + juce::String::charToString(rank);
		}

		int m_Row;
		int m_Col;
		uint8_t m_Rank;
		uint8_t m_File;
		juce::String m_Id;
		juce::Colour m_SquareColor;
		bool m_IsCandidate;

	};

	class Piece : public juce::ImageButton
	{
	public:
		Piece(const char *resourceName, uint8_t name, juce::String intialSquareId) : m_Name(name), m_SquareId(intialSquareId), m_Team( (isupper(name) ? Chess::Color::White : Chess::Color::Black))
		{
			int size;
			const char *data = ChessImageData::getNamedResource(resourceName, size);
			m_Image = juce::ImageFileFormat::loadFrom(data, size);
			// m_Image = juce::ImageFileFormat::loadFrom(imageFile);
			setImages(false, true, true, m_Image, 1, juce::Colours::transparentBlack, juce::Image(nullptr), 0.5, juce::Colours::transparentWhite, juce::Image(nullptr), 0.5, juce::Colours::transparentWhite);
			setSize(80, 80);
		};
		~Piece() = default;

		juce::String getId() const { return juce::String::charToString(m_Name); };
		void setSquareId(juce::String newId) { m_SquareId = newId; };
		juce::String getSquareId() const { return m_SquareId; };
		Chess::Color getTeam() const { return m_Team; };
		bool operator==(const Piece& lhs) const
		{
			return (this->m_Name == lhs.m_Name && this->m_SquareId == lhs.m_SquareId);
		}

		bool isAlly(const Piece& piece) const
		{
			return m_Team == piece.m_Team;
		}

		void placeAt(const Square* square)
		{
			m_Square = square;
			m_SquareId = square->getId();
			setBounds(square->getBounds());
		}

		void resized()
		{
			if (m_Square)
				setBounds(m_Square->getBounds());
		}

		void setSelected(bool isSelected)
		{
			mIsSelected = isSelected;
		}

		void mouseDown(const juce::MouseEvent& event) override
		{
			ImageButton::mouseDown(event);
		}

		void mouseDrag(const juce::MouseEvent& event) override
		{
			if (!mIsSelected)
			{
				triggerClick();
			}
			juce::MouseEvent relEvent = event.getEventRelativeTo(getParentComponent());
			setCentrePosition(relEvent.getPosition());
			mWasBeingDragged = true;
		}

		void mouseUp(const juce::MouseEvent& event) override
		{

			if (mWasBeingDragged)
			{
				for (juce::Component* component : getParentComponent()->getChildren())
				{
					if (component != this && component->getBoundsInParent().contains(event.getEventRelativeTo(getParentComponent()).getPosition()))
					{
						component->mouseDown(event);
						component->mouseUp(event);
					}
				}
				mWasBeingDragged = false;
			}
			else
			{
				ImageButton::mouseUp(event);
			}
			resized();
		}

	private:

		bool mIsSelected = false;
		bool mWasBeingDragged = false;
		const Square* m_Square = nullptr;
		juce::Image m_Image;
		uint8_t m_Name;
		juce::String m_SquareId;
		const Chess::Color m_Team = Chess::Color::White;
	};

	class ChessBoard : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster, public juce::ChangeListener, public juce::ActionListener
	{
	public:

		enum class state {
			kIdle,
			kPlacing,
			kSwitching,

			kNumStates
		};

		enum class mode {
			kPVP,
			kPVC,
			kPGN,

			kNumModes
		};

		ChessBoard()
		{
			auto genColor = [](int row, int col) {
				if (row % 2 == 0)
				{
					if (col % 2 == 0) return juce::Colour(240, 217, 181);
					else return juce::Colour(181, 136, 99);
				}
				else
				{
					if (col % 2 == 0) return juce::Colour(181, 136, 99);
					else return juce::Colour(240, 217, 181);
				}
			};

			for (int row = 0; row < BoardSize; row++)
			{
				for (int col = 0; col < BoardSize; col++)
				{
					Square* square = new Square(row, col, genColor(row, col));
					addAndMakeVisible(square);
					square->addListener(this);
					square->addActionListener(this);
					m_AllSquares[row][col] = square;
				}
			}

			for (Piece& piece : m_AllPieces)
			{
				addAndMakeVisible(piece);
				piece.placeAt(findSquare(piece));
				piece.addListener(this);
			}

		};

		~ChessBoard()
		{
			for (int row = 0; row < BoardSize; row++)
			{
				for (int col = 0; col < BoardSize; col++)
				{
					Square*& square = m_AllSquares[row][col];
					square->removeListener(this);
					square->removeActionListener(this);
					delete square;
					square = nullptr;
				}
			}

			for (Piece& piece : m_AllPieces)
				piece.removeListener(this);
		}

		void paint(juce::Graphics& g) override
		{

		}

		void resized() override
		{
			auto area = getBounds();
			int squareHeight = area.getHeight() / BoardSize;
			int squareWidth = area.getWidth() / BoardSize;

			int ypos = area.getHeight();
			for (int row = 0; row < BoardSize; row++)
			{
				ypos -= squareHeight;
				int xpos = 0;
				for (int col = 0; col < BoardSize; col++)
				{
					m_AllSquares[row][col]->setBounds(xpos, ypos, squareWidth, squareHeight);
					xpos += squareWidth;
				}
			}

			for (Piece& piece : m_AllPieces)
			{
				piece.resized();
			}
		}

		void buttonClicked(juce::Button* button) override
		{
			if (m_CurrentMode != ChessBoard::mode::kPGN)
			{
				for (Piece& piece : m_AllPieces)
				{
					if (button == &piece)
					{
						if (m_SelectedPiece)
						{
							if (m_SelectedPiece == &piece)
							{
								onStateChange(state::kIdle);
							}
							else if (m_SelectedPiece->isAlly(piece))
							{
								onStateChange(state::kSwitching);
								m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
								selectPiece(piece);
							}
							else
							{
								juce::String intent = m_SelectedPiece->getSquareId() + piece.getSquareId();
								sendActionMessage(intent);
							}
						}
						else {
							if (m_CurrentMode == ChessBoard::mode::kPVP)
							{
								if (AppState::getInstance().getGame().getTurn() == piece.getTeam())
								{
									selectPiece(piece);
									onStateChange(state::kPlacing);
								}
							}
							else 
							{
								if (AppState::getInstance().getGame().getTurn() == Chess::Color::White && piece.getTeam() == Chess::Color::White)
								{
									selectPiece(piece);
									onStateChange(state::kPlacing);
								}
							}

						}
						return;
					}

				}

				for (int row = 0; row < BoardSize; row++)
				{
					for (int col = 0; col < BoardSize; col++)
					{
						Square*& square = m_AllSquares[row][col];
						if (button == square)
						{
							if (m_CurrentState == state::kPlacing)
							{
								juce::String intent = m_SelectedPiece->getSquareId() + square->getId();
								sendActionMessage(intent);
							}
							return;
						}
					}
				}
			}
			
		}

		void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
				std::list<Piece*> pieceList;
				for (Piece& piece : m_AllPieces)
				{
					piece.setVisible(false);
					pieceList.push_front(&piece);
				}

				auto pieceMap = AppState::getInstance().getGame().getPieceMap();
				for (const auto [square, piece] : pieceMap)
				{
					for (Piece* guiPiece : pieceList)
					{
						if (guiPiece->getId().toStdString() == std::string{ piece.toChar() })
						{
							guiPiece->setVisible(true);
							guiPiece->placeAt(findSquare(square.toString()));
							pieceList.remove(guiPiece);
							break;
						}

					}
				}
				onStateChange(state::kIdle);
		}

		void actionListenerCallback(const juce::String& message) override
		{
			if (message.contains("Preview"))
			{
				if (m_SelectedPiece)
				{
					juce::String newMessage = message.substring(0, 8) + m_SelectedPiece->getSquareId() + message.substring(8, 10);
					sendActionMessage(newMessage);
				}

			}
		}

		void onModeChange(ChessBoard::mode newMode)
		{
			switch (newMode)
			{
			case ChessBoard::mode::kPVP:
				break;
			case ChessBoard::mode::kPVC:
				break;
			default:
				;
			}
			m_CurrentMode = newMode;
		}

	private:

		static constexpr int BoardSize = 8;
		static constexpr int NumPieces = 32;

		ChessBoard::mode m_CurrentMode = ChessBoard::mode::kPVP;
		ChessBoard::state m_CurrentState = ChessBoard::state::kIdle;
		Piece* m_SelectedPiece = nullptr;
		Square* m_AllSquares[BoardSize][BoardSize]{ nullptr };

		//Change this to point where your images relative to your working directory
		const juce::File pathToImages = juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/MUSI-6106/chessImages");

		Piece m_AllPieces[32]{
			Piece {"W_Rook_png", 'R', "a1"},
			Piece {"W_Knight_png", 'N', "b1"},
			Piece {"W_Bishop_png", 'B', "c1"},
			Piece {"W_Queen_png", 'Q', "d1"},
			Piece {"W_King_png", 'K', "e1"},
			Piece {"W_Bishop_png", 'B', "f1"},
			Piece {"W_Knight_png", 'N', "g1"},
			Piece {"W_Rook_png", 'R', "h1"},
			Piece {"W_Pawn_png", 'P', "a2"},
			Piece {"W_Pawn_png", 'P', "b2"},
			Piece {"W_Pawn_png", 'P', "c2"},
			Piece {"W_Pawn_png", 'P', "d2"},
			Piece {"W_Pawn_png", 'P', "e2"},
			Piece {"W_Pawn_png", 'P', "f2"},
			Piece {"W_Pawn_png", 'P', "g2"},
			Piece {"W_Pawn_png", 'P', "h2"},
			Piece {"B_Pawn_png", 'p', "a7"},
			Piece {"B_Pawn_png", 'p', "b7"},
			Piece {"B_Pawn_png", 'p', "c7"},
			Piece {"B_Pawn_png", 'p', "d7"},
			Piece {"B_Pawn_png", 'p', "e7"},
			Piece {"B_Pawn_png", 'p', "f7"},
			Piece {"B_Pawn_png", 'p', "g7"},
			Piece {"B_Pawn_png", 'p', "h7"},
			Piece {"B_Rook_png", 'r', "a8"},
			Piece {"B_Knight_png", 'n', "b8"},
			Piece {"B_Bishop_png", 'b', "c8"},
			Piece {"B_Queen_png", 'q', "d8"},
			Piece {"B_King_png", 'k', "e8"},
			Piece {"B_Bishop_png", 'b', "f8"},
			Piece {"B_Knight_png", 'n', "g8"},
			Piece {"B_Rook_png", 'r', "h8"}
		};

		void selectPiece(Piece& piece)
		{
			if (m_SelectedPiece)
			{
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

		Square* findSquare(const juce::String squareId) const
		{
			for (int row = 0; row < BoardSize; row++)
			{
				for (int col = 0; col < BoardSize; col++)
				{
					Square* square = m_AllSquares[row][col];
					if (squareId == square->getId())
						return square;
				}
			}
		}

		Square* findSquare(const Piece& piece) const
		{
			return findSquare(piece.getSquareId());
		}

		void highlightPossibleMoves(const Piece* piece)
		{
			if (piece)
			{
				Chess::Game game = AppState::getInstance().getGame();
				std::unordered_set moves = game.generateMoves(Chess::Square(piece->getSquareId().toStdString()));
				for (const Chess::Move& move : moves)
				{
					Square* square = findSquare(move.dst.toString());
					square->isCandidate(true);
				}
			}

		}

		void resetPossibleMoves()
		{
			for (int row = 0; row < BoardSize; row++)
			{
				for (int col = 0; col < BoardSize; col++)
				{
					Square*& square = m_AllSquares[row][col];
					square->isCandidate(false);
				}
			}
		}

		void onStateChange(ChessBoard::state newState)
		{
			switch (newState)
			{
			case ChessBoard::state::kIdle:
				if (m_SelectedPiece)
				{
					m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
					m_SelectedPiece->setSelected(false);
					sendActionMessage("Deselect " + m_SelectedPiece->getId());
				}
				m_SelectedPiece = nullptr;
				resetPossibleMoves();
				break;
			case ChessBoard::state::kSwitching:
				resetPossibleMoves();
				break;
			default:
				;
			}

			m_CurrentState = newState;
		}

	};
}

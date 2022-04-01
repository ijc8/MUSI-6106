#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "GameState.h"

namespace GUI
{
	class Square : public juce::Button
	{
	public:
		Square(int row, int column, juce::Colour color) : juce::Button("square"), m_SquareColor(color), m_Row(row), m_Col(column)
		{
			setToggleable(true);
			setClickingTogglesState(true);
			m_Rank = toRank(row);
			m_File = toFile(column);
			setId(m_Rank, m_File);
			setSize(80, 80);
		};

		~Square() = default;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
		{
			if (shouldDrawButtonAsHighlighted)
				g.fillAll(juce::Colours::yellow);
			else if (shouldDrawButtonAsDown)
				g.fillAll(juce::Colours::blue);
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

	};

	class Piece : public juce::ImageButton
	{
	public:
		Piece(juce::File imageFile, uint8_t name, juce::String intialSquareId) : m_Name(name), m_SquareId(intialSquareId), m_Team( (isupper(name) ? Chess::Color::White : Chess::Color::Black))
		{
			m_Image = juce::ImageFileFormat::loadFrom(imageFile);
			setImages(false, true, true, m_Image, 1, juce::Colours::transparentBlack, juce::Image(nullptr), 0.5, juce::Colours::transparentWhite, juce::Image(nullptr), 0.5, juce::Colours::yellow);
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

	private:

		const Square* m_Square = nullptr;
		juce::Image m_Image;
		uint8_t m_Name;
		juce::String m_SquareId;
		const Chess::Color m_Team = Chess::Color::White;
	};

	class ChessBoard : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster, public juce::ChangeListener
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
		//const juce::File pathToImages = juce::File::getCurrentWorkingDirectory().getParentDirectory().getChildFile("MUSI-6106/chessImages");
		const juce::File pathToImages = juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/MUSI-6106/chessImages");

		Piece m_AllPieces[32]{
			Piece { pathToImages.getChildFile("W_Rook.png"), 'R', "a1"},
			Piece { pathToImages.getChildFile("W_Knight.png"), 'N', "b1"},
			Piece { pathToImages.getChildFile("W_Bishop.png"), 'B', "c1"},
			Piece { pathToImages.getChildFile("W_Queen.png"), 'Q', "d1"},
			Piece { pathToImages.getChildFile("W_King.png"), 'K', "e1"},
			Piece { pathToImages.getChildFile("W_Bishop.png"), 'B', "f1"},
			Piece { pathToImages.getChildFile("W_Knight.png"), 'N', "g1"},
			Piece { pathToImages.getChildFile("W_Rook.png"), 'R', "h1"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "a2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "b2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "c2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "d2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "e2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "f2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "g2"},
			Piece { pathToImages.getChildFile("W_Pawn.png"), 'P', "h2"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "a7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "b7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "c7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "d7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "e7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "f7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "g7"},
			Piece { pathToImages.getChildFile("B_Pawn.png"), 'p', "h7"},
			Piece { pathToImages.getChildFile("B_Rook.png"), 'r', "a8"},
			Piece { pathToImages.getChildFile("B_Knight.png"), 'n', "b8"},
			Piece { pathToImages.getChildFile("B_Bishop.png"), 'b', "c8"},
			Piece { pathToImages.getChildFile("B_Queen.png"), 'q', "d8"},
			Piece { pathToImages.getChildFile("B_King.png"), 'k', "e8"},
			Piece { pathToImages.getChildFile("B_Bishop.png"), 'b', "f8"},
			Piece { pathToImages.getChildFile("B_Knight.png"), 'n', "g8"},
			Piece { pathToImages.getChildFile("B_Rook.png"), 'r', "h8"}
		};

		void selectPiece(Piece& piece)
		{
			m_SelectedPiece = &piece;
			piece.setToggleState(true, juce::dontSendNotification);
			highlightPossibleMoves(piece);
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

		void highlightPossibleMoves(const Piece& piece)
		{
			Chess::Game game = AppState::getInstance().getGame();
			std::unordered_set moves = game.generateMoves(Chess::Square(piece.getSquareId().toStdString()));
			for (const Chess::Move& move : moves)
			{
				Square* square = findSquare(move.dst.toString());
				square->setToggleState(true, juce::sendNotification);
			}
		}

		void resetPossibleMoves()
		{
			for (int row = 0; row < BoardSize; row++)
			{
				for (int col = 0; col < BoardSize; col++)
				{
					Square*& square = m_AllSquares[row][col];
					square->setToggleState(false, juce::sendNotification);
				}
			}
		}

		void onStateChange(ChessBoard::state newState)
		{
			switch (newState)
			{
			case ChessBoard::state::kIdle:
				if (m_SelectedPiece) m_SelectedPiece->setToggleState(false, juce::dontSendNotification);
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
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
		Piece(juce::File imageFile, uint8_t name, juce::String intialSquareId) : m_Name(name), m_SquareId(intialSquareId)
		{
			m_Image = juce::ImageFileFormat::loadFrom(imageFile);
			setImages(false, true, true, m_Image, 1, juce::Colours::transparentBlack, juce::Image(nullptr), 0.5, juce::Colours::transparentWhite, juce::Image(nullptr), 0.5, juce::Colours::yellow);
			setSize(80, 80);
		};
		~Piece() = default;

		juce::String getId() const { return juce::String::charToString(m_Name); };
		void setSquareId(juce::String newId) { m_SquareId = newId; };
		juce::String getSquareId() const { return m_SquareId; };
		bool operator==(const Piece& lhs) const
		{
			return (this->m_Name == lhs.m_Name && this->m_SquareId == lhs.m_SquareId);
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
	};

	class ChessBoard : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster, public juce::ChangeListener
	{
	public:

		enum class state {
			kIdle,
			kPlacing
		};

		ChessBoard()
		{
			auto genColor = [](int row, int col) {
				if (row % 2 == 0)
				{
					if (col % 2 == 0) return juce::Colours::black;
					else return juce::Colours::white;
				}
				else
				{
					if (col % 2 == 0) return juce::Colours::white;
					else return juce::Colours::black;
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
			for (Piece& piece : m_AllPieces)
			{
				if (button == &piece)
				{
					m_SelectedPiece = &piece;
					m_CurrentState = state::kPlacing;
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

		void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
			if (source == &AppState::getInstance().getGame())
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


			}
		}




	private:

		static constexpr int BoardSize = 8;
		static constexpr int NumPieces = 32;

		ChessBoard::state m_CurrentState = ChessBoard::state::kIdle;
		Piece* m_SelectedPiece = nullptr;
		Square* m_AllSquares[BoardSize][BoardSize]{ nullptr };
		Piece m_AllPieces[32]{
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Rook.png"), 'R', "a1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Knight.png"), 'N', "b1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Bishop.png"), 'B', "c1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Queen.png"), 'Q', "d1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_King.png"), 'K', "e1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Bishop.png"), 'B', "f1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Knight.png"), 'N', "g1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Rook.png"), 'R', "h1"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "a2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "b2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "c2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "d2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "e2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "f2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "g2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/W_Pawn.png"), 'P', "h2"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "a7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "b7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "c7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "d7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "e7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "f7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "g7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Pawn.png"), 'p', "h7"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Rook.png"), 'r', "a8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Knight.png"), 'n', "b8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Bishop.png"), 'b', "c8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Queen.png"), 'q', "d8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_King.png"), 'k', "e8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Bishop.png"), 'b', "f8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Knight.png"), 'n', "g8"},
			Piece { juce::File("C:/Users/JohnK/Documents/ASE/MusicalChess/chessImages/B_Rook.png"), 'r', "h8"}
		};

		const Square* findSquare(juce::String squareId) const
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


		const Square* findSquare(const Piece& piece) const
		{
			return findSquare(piece.getSquareId());
		}

		void onStateChange(ChessBoard::state newState)
		{
			switch (newState)
			{
			case ChessBoard::state::kIdle:
				m_SelectedPiece = nullptr;
				break;
			case ChessBoard::state::kPlacing:
				break;
			}
			m_CurrentState = newState;
		}

	};
}
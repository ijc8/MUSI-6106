#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace Chess {
    enum class Color {
        White,
        Black
    };

    struct Piece {
        enum class Type {
            Pawn = 'P',
            Knight = 'N',
            Bishop = 'B',
            Rook = 'R',
            Queen = 'Q',
            King = 'K',
        };

        static const std::unordered_map<char, Type> CharMap;
        static const std::unordered_map<Type, char> ToChar;

        // Default constructor is required for use in a map.
        Piece(): Piece(Type::Pawn, Color::White) {}
        Piece(Type type_, Color color_) : type(type_), color(color_) {}
        Type type;
        Color color;
    };

    struct Square {
        Square(uint8_t rank_, uint8_t file_) : rank(rank_), file(file_) {}
        bool operator==(const Square &other) const {
            return rank == other.rank && file == other.file;
        }
        uint8_t rank, file;
    };

    struct Move {
        Move(Square src_, Square dst_, std::optional<Piece::Type> promotion_ = std::nullopt)
            : src(src_), dst(dst_), promotion(promotion_) {}
        Square src, dst;
        std::optional<Piece::Type> promotion;
    };
}

// Implement hash for Square so that we create a map of Square -> Piece.
template <>
struct std::hash<Chess::Square> {
    std::size_t operator()(const Chess::Square &square) const {
        // Rank and file can take on values from 0-7 (inclusive), so each takes up 3 bits.
        return (uint8_t)(square.rank << 3) | square.file;
    }
};

namespace Chess {
    class Board {
    public:
        static const std::string initialBoardFen;

        Board(const std::string boardFen = initialBoardFen);

        std::string getBoardFen() const;
        void setBoardFen(const std::string boardFen);

        std::optional<Piece> getPieceAt(const Square square) const;
        void setPieceAt(const Square square, const std::optional<Piece>);
        std::unordered_map<Square, Piece> getPieceMap() const;

        // TODO: Implement move history & keep track of other state like turn, castling rights, etc; maybe in another class.
        // void push(Move move);
        // Move pop();
        // Move peek();
    private:
        std::optional<Piece> board[8][8];
        std::unordered_map<Square, Piece> pieceMap;
    };
}

#endif

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <cassert>
#include <cstdint>
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Chess {
    enum class Color {
        White,
        Black
    };

    struct Piece {
        enum class Type {
            Pawn, Knight, Bishop, Rook, Queen, King
        };

        static const std::unordered_map<char, Type> FromChar;
        static const std::unordered_map<Type, char> ToChar;

        // Default constructor is required for use in a map.
        Piece(): Piece(Type::Pawn, Color::White) {}
        Piece(Type type_, Color color_) : type(type_), color(color_) {}
        Piece(char c) : type(FromChar.at(toupper(c))), color(islower(c) ? Color::Black : Color::White) {}
        bool operator==(const Piece& other) const {
            return type == other.type && color == other.color;
        }
        char toChar() const {
            char typeChar = ToChar.at(type);
            return color == Color::Black ? tolower(typeChar) : typeChar;
        }
        Type type;
        Color color;
    };

    struct Square {
        Square(uint8_t rank_, uint8_t file_) : rank(rank_), file(file_) {}
        Square(const std::string &square) : Square(square[1] - '1', square[0] - 'a') {}
        bool operator==(const Square &other) const {
            return rank == other.rank && file == other.file;
        }
        std::string toString() const {
            return std::string{(char)(file + 'a'), (char)(rank + '1')};
        }
        uint8_t rank, file;
    };

    struct Move {
        Move(Square src_, Square dst_, std::optional<Piece::Type> promotion_ = std::nullopt)
            : src(src_), dst(dst_), promotion(promotion_) {}
        Move(const std::string &squares)  // Build from string of form "b1c3".
            : Move(Square(squares.substr(0, 2)), Square(squares.substr(2))) {}
        bool operator==(const Move &other) const {
            return src == other.src && dst == other.dst && promotion == other.promotion;
        }
        std::string toString() const {
            return src.toString() + dst.toString();
        }
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

template <>
struct std::hash<Chess::Move> {
    std::size_t operator()(const Chess::Move &move) const {
        uint8_t src = std::hash<Chess::Square>{}(move.src); // 6 bits
        uint8_t dst = std::hash<Chess::Square>{}(move.dst); // 6 bits
        // Pawn is invalid promotion type, so we use 0 to represent no promotion.
        uint8_t promotion = move.promotion.has_value() ? (int)*move.promotion : 0; // 5 possibilities: 3 bits
        return (std::size_t)(promotion << 12 | dst << 6 | src);
    }
};

namespace Chess {
    class Board {
    public:
        static const std::string initialBoardFen;

        Board(const std::string &boardFen = initialBoardFen);

        std::string getBoardFen() const;
        void setBoardFen(const std::string &boardFen);

        std::optional<Piece> getPieceAt(Square square) const;
        void setPieceAt(Square square, std::optional<Piece>);
        std::unordered_map<Square, Piece> getPieceMap() const;
        std::unordered_set<Square> getPieces(Piece piece) const;

    protected:
        std::optional<Piece> board[8][8];
        std::unordered_map<Square, Piece> pieceMap;
    };

    struct PlayerCastleRights {
        bool kingSide;
        bool queenSide;
    };

    struct CastleRights {
        PlayerCastleRights white;
        PlayerCastleRights black;
    };

    class GameState: public Board {
    public:
        static const std::string initialFen;

        GameState(const std::string &fen = initialFen);

        std::string getFen() const;
        void setFen(const std::string &fen);

        Color getTurn() const { return turn; }
        std::optional<Square> getEnPassant() const { return enPassant; }
        bool canCastle(Piece castleType) const {
            PlayerCastleRights rights = (castleType.color == Color::White ? castleRights.white : castleRights.black);
            if (castleType.type == Piece::Type::King) {
                return rights.kingSide;
            } else if (castleType.type == Piece::Type::Queen) {
                return rights.queenSide;
            }
            assert(false);
            return false;
        }
        int getHalfmoveClock() const { return halfmoveClock; }
        int getFullmoveNumber() const { return fullmoveNumber; }

        std::unordered_set<Move> generateMoves(Square src) const;
        bool isCheck(Color color) const;
        std::optional<std::optional<Color>> getOutcome() const;
        std::vector<Chess::Move> generateLegalMoves() const;
        bool wouldBeInCheck(Move move) const;
        bool isLegal(Move move) const;

        std::unordered_map<Square, std::optional<Piece>> getThreats();
        std::unordered_map<Square, std::optional<Piece>> getAttackers();

        void execute(Move move);

    protected:
        Color turn;
        std::optional<Square> enPassant;
        CastleRights castleRights;
        int halfmoveClock = 0;
        int fullmoveNumber = 0;
    };

    class Game: public GameState {
    public:
        using GameState::GameState;
        void push(Move move);
        Move pop();

    private:
        std::stack<std::tuple<Move, GameState>> history;
    };
}

// TODO: Move to separate module as this grows to include other application state.
class AppState {
public:
    static AppState &getInstance() {
        static AppState instance;
        return instance;
    }

    Chess::Game &getGame() {
        return game;
    }
private:
    Chess::Game game;
};

#endif

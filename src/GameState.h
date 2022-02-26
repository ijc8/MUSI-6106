#ifndef GAME_STATE_H
#define GAME_STATE_H

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
            Pawn, Knight, Bishop, Rook, Queen, King,
        };

        static const std::unordered_map<char, Type> FromChar;
        static const std::unordered_map<Type, char> ToChar;

        // Default constructor is required for use in a map.
        Piece(): Piece(Type::Pawn, Color::White) {}
        Piece(const Type type_, const Color color_) : type(type_), color(color_) {}
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
        Square(const std::string square) : Square(square[1] - '1', square[0] - 'a') {}
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
        bool operator==(const Move &other) const {
            return src == other.src && dst == other.dst && promotion == other.promotion;
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

        Board(const std::string boardFen = initialBoardFen);

        std::string getBoardFen() const;
        void setBoardFen(const std::string boardFen);

        std::optional<Piece> getPieceAt(const Square square) const;
        void setPieceAt(const Square square, const std::optional<Piece>);
        std::unordered_map<Square, Piece> getPieceMap() const;
        std::unordered_set<Square> getPieces(const Piece piece) const;

    protected:
        std::optional<Piece> board[8][8];
        std::unordered_map<Square, Piece> pieceMap;
    };

    struct CastleRights {
        bool whiteShort;
        bool whiteLong;
        bool blackShort;
        bool blackLong;
    };

    class GameState: public Board {
    public:
        static const std::string initialFen;

        GameState(const std::string fen = initialFen);

        std::string getFen() const;
        void setFen(const std::string fen);

        Color getTurn() const { return turn; }
        std::optional<Square> getEnPassant() const { return enPassant; }
        bool canCastle(Piece castleType) const {
            if (castleType == Piece('K')) {
                return castleRights.whiteShort;
            } else if (castleType == Piece('Q')) {
                return castleRights.whiteLong;
            } else if (castleType == Piece('k')) {
                return castleRights.blackShort;
            } else if (castleType == Piece('q')) {
                return castleRights.blackLong;
            }
            assert(false);
            return false;
        }
        int getHalfmoveClock() const { return halfmoveClock; }
        int getFullmoveNumber() const { return fullmoveNumber; }

        std::unordered_set<Move> generateMoves(Square src) const;
        bool isCheck(Color color) const;
        bool isLegal(Move move) const;
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

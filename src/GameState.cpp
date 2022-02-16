#include <cassert>
#include <cctype>
#include <sstream>

#include "GameState.h"

using namespace Chess;

// Starting position for a game of chess using standard rules.
const std::string Board::initialBoardFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
// Starting state for a game of chess using standard rules.
const std::string GameState::initialFen = Board::initialBoardFen + " w KQkq - 0 1";

const std::unordered_map<char, Piece::Type> Piece::CharMap{
    {'P', Piece::Type::Pawn},
    {'B', Piece::Type::Bishop},
    {'N', Piece::Type::Knight},
    {'R', Piece::Type::Rook},
    {'Q', Piece::Type::Queen},
    {'K', Piece::Type::King},
};

Board::Board(std::string boardFen) {
    setBoardFen(boardFen);
}

std::string Board::getBoardFen() const {
    std::string fen;
    // Counts number of consecutive empty squares in a rank.
    int emptyCount = 0;
    // Helper function.
    auto flushEmpty = [&emptyCount, &fen]() {
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount);
            emptyCount = 0;
        }
    };

    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            std::optional<Piece> piece = getPieceAt(Square(rank, file));
            if (piece.has_value()) {
                flushEmpty();
                char pieceChar = (char)piece.value().type;
                fen += (piece.value().color == Color::White ? pieceChar : tolower(pieceChar));
            } else {
                emptyCount++;
            }
        }
        flushEmpty();
        // Insert slash between ranks.
        if (rank > 0) {
            fen += '/';
        }
    }
    return fen;
}

void Board::setBoardFen(const std::string boardFen) {
    int rank = 7, file = 0;
    for (const char c : boardFen) {
        assert(rank >= 0 && rank < 8 && file >= 0 && (file < 8 || c == '/'));
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            int endFile = file + (c - '0');
            for (; file < endFile; file++) {
                setPieceAt(Square(rank, file), std::nullopt);
            }
        } else {
            setPieceAt(Square(rank, file), Piece(c));
            file++;
        }
    }
}

std::optional<Piece> Board::getPieceAt(const Square square) const {
    return board[square.rank][square.file];
}

void Board::setPieceAt(const Square square, const std::optional<Piece> piece) {
    board[square.rank][square.file] = piece;
    if (piece.has_value()) {
        pieceMap[square] = piece.value();
    } else {
        pieceMap.erase(square);
    }
}

std::unordered_map<Square, Piece> Board::getPieceMap() const {
    return pieceMap;
}


GameState::GameState(const std::string fen) {
    setFen(fen);
}

std::string GameState::getFen() const {
    std::string boardFen = getBoardFen();
    char turnc = turn == Color::White ? 'w' : 'b';
    std::string castleFen = "";
    for (char p : std::string("KQkq")) {
        if (canCastle(Piece(p))) {
            castleFen += p;
        }
    }
    if (castleFen.empty()) {
        castleFen = '-';
    }
    std::string ep = enPassant.has_value() ? enPassant.value().toString() : "-";
    std::string halfmove = std::to_string(halfmoveClock);
    std::string fullmove = std::to_string(fullmoveNumber);
    return boardFen + " " + turnc + " " + castleFen + " " + ep + " " + halfmove + " " + fullmove;
}

void GameState::setFen(const std::string fen) {
    std::istringstream stream(fen);
    std::string boardFen, castleFen, ep;
    char turnc;
    stream >> boardFen >> turnc >> castleFen >> ep >> halfmoveClock >> fullmoveNumber;
    setBoardFen(boardFen);
    turn = turnc == 'w' ? Color::White : Color::Black;
    for (char p : castleFen) {
        if (p == 'K') {
            castleRights.whiteShort = true;
        } else if (p == 'Q') {
            castleRights.whiteLong = true;
        } else if (p == 'k') {
            castleRights.blackShort = true;
        } else if (p == 'q') {
            castleRights.blackLong = true;
        }
    }
    enPassant = ep == "-" ? std::nullopt : std::make_optional(Square(ep));
}

void Game::push(Move move) {
    // Push the move and a copy of the game state on to the history stack.
    history.emplace(move, GameState(*this));
    // NOTE: This does not check `move` for legality.
    Piece piece = getPieceAt(move.src).value();
    setPieceAt(move.src, std::nullopt);
    if (move.promotion.has_value()) {
        setPieceAt(move.dst, Piece(move.promotion.value(), piece.color));
    } else {
        setPieceAt(move.dst, piece);
    }

    // Flip turn.
    turn = turn == Color::White ? Color::Black : Color::White;
    // Handle en passant.
    if (piece.type == Piece::Type::Pawn && abs(move.src.rank - move.dst.rank) == 2) {
        enPassant.emplace((move.src.rank + move.dst.rank) / 2, move.src.file);
    }
    // TODO: Update move clocks, castling rights.
}

Move Game::pop() {
    auto [move, state] = history.top();
    // TODO: Restore game state in a less hacky way.
    // (or, just store FEN strings in the history instead of GameStates.)
    setFen(state.getFen());
    return move;
}

#include <cassert>
#include <cctype>

#include "GameState.h"

using namespace Chess;

// Starting state for a game of chess using standard rules.
const std::string Board::initialBoardFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

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
            file += c - '0';
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

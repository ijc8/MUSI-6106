#include "GameState.h"

using namespace Chess;

// Starting state for a game of chess using standard rules.
const std::string Board::initialBoardFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

Board::Board(std::string boardFen) {
    setBoardFen(boardFen);
}

std::string Board::getBoardFen() const {
    throw "not implemented";
}

void Board::setBoardFen(const std::string boardFen) {
    throw "not implemented";
}

std::optional<Piece> Board::getPieceAt(const Square square) const {
    return board[square.rank][square.file];
}

void Board::setPieceAt(const Square square, const std::optional<Piece> piece) {
    board[square.rank][square.file] = piece;
    // if (piece.has_value()) {
    //     pieceMap[square] = piece.value();
    // } else {
    //     pieceMap.erase(square);
    // }
}

std::unordered_map<Square, Piece> Board::getPieceMap() const {
    return pieceMap;
}

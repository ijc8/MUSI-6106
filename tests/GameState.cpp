#include <cassert>
#include <iostream>

#include "../src/GameState.h"

using namespace Chess;

int main() {
    Board board;
    std::cout << "FEN: " << board.getBoardFen() << std::endl;

    assert(board.getBoardFen() == Board::initialBoardFen);
    assert(board.getPieceAt(Square(0, 0)) == Piece(Piece::Type::Rook, Color::White));
    assert(Square("e7") == Square(6, 4));
    assert(Piece('p') == Piece(Piece::Type::Pawn, Color::Black));
    assert(board.getPieceAt(Square("e7")) == Piece('p'));

    // Ensure singleton works as expected.
    AppState &state = AppState::getInstance();
    state.getGame().setPieceAt(Square("h4"), Piece('Q'));

    AppState &state2 = AppState::getInstance();
    assert(&state == &state2);
    assert(state2.getGame().getPieceAt(Square("h4")) == Piece('Q'));

    return 0;
}

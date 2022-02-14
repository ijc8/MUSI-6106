#include <cassert>
#include <iostream>

#include "../src/GameState.h"

using namespace Chess;

int main() {
    // Test basic board (positions only).
    Board board;
    std::cout << "Board FEN: " << board.getBoardFen() << std::endl;

    assert(board.getBoardFen() == Board::initialBoardFen);
    assert(board.getPieceAt(Square(0, 0)) == Piece(Piece::Type::Rook, Color::White));
    assert(Square("e7") == Square(6, 4));
    assert(Piece('p') == Piece(Piece::Type::Pawn, Color::Black));
    assert(board.getPieceAt(Square("e7")) == Piece('p'));

    // Test game state (complete description of immediate state with no history).
    GameState game;
    std::cout << "FEN: " << game.getFen() << std::endl;
    assert(game.getFen() == GameState::initialFen);
    assert(game.getTurn() == Color::White);
    assert(game.canCastle('K') == true);

    game.setFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    assert(game.getPieceAt(Square("e4")) == Piece('P'));
    assert(game.getTurn() == Color::Black);
    assert(game.canCastle(Piece('K')) && game.canCastle(Piece('Q')) && game.canCastle(Piece('k')) && game.canCastle(Piece('q')));
    assert(game.getEnPassant().has_value() && game.getEnPassant().value() == Square("e3"));
    assert(game.getHalfmoveClock() == 0);
    assert(game.getFullmoveNumber() == 1);

    // Ensure singleton works as expected.
    AppState &state = AppState::getInstance();
    state.getGame().setPieceAt(Square("h4"), Piece('Q'));

    AppState &state2 = AppState::getInstance();
    assert(&state == &state2);
    assert(state2.getGame().getPieceAt(Square("h4")) == Piece('Q'));

    return 0;
}

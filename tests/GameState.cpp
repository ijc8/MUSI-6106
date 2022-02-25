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
    GameState gameState;
    std::cout << "FEN: " << gameState.getFen() << std::endl;
    assert(gameState.getFen() == GameState::initialFen);
    assert(gameState.getTurn() == Color::White);
    assert(gameState.canCastle('K') == true);

    gameState.setFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    assert(gameState.getPieceAt(Square("e4")) == Piece('P'));
    assert(gameState.getTurn() == Color::Black);
    assert(gameState.canCastle(Piece('K')) && gameState.canCastle(Piece('Q')) && gameState.canCastle(Piece('k')) && gameState.canCastle(Piece('q')));
    assert(gameState.getEnPassant().has_value() && gameState.getEnPassant().value() == Square("e3"));
    assert(gameState.getHalfmoveClock() == 0);
    assert(gameState.getFullmoveNumber() == 1);

    // Test game (complete description of a chess game including move history).
    Game game;
    // Push a move and ensure that the state has updated. (Might be nice to support algebraic notation here.)
    game.push(Move(Square("e2"), Square("e4")));
    assert(game.getFen() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    assert(game.getPieceAt(Square("e4")).value() == Piece('P'));
    assert(!game.getPieceAt(Square("e2")).has_value());
    // Confirm that popping restores previous state.
    assert(game.pop() == Move(Square("e2"), Square("e4")));
    assert(game.getFen() == GameState::initialFen);
    // Confirm that history is LIFO and en passant works as expected:
    game.push(Move(Square("d2"), Square("d4")));
    assert(game.getEnPassant() == Square("d3"));
    game.push(Move(Square("b8"), Square("c6")));
    assert(game.getEnPassant() == std::nullopt);
    assert(game.pop() == Move(Square("b8"), Square("c6")));
    assert(game.pop() == Move(Square("d2"), Square("d4")));

    assert(game.getFen() == GameState::initialFen);
    assert(game.isLegal(Move(Square("e2"), Square("e4"))));
    assert(!game.isLegal(Move(Square("e2"), Square("e5"))));

    // Ensure singleton works as expected.
    AppState &state = AppState::getInstance();
    state.getGame().setPieceAt(Square("h4"), Piece('Q'));

    AppState &state2 = AppState::getInstance();
    assert(&state == &state2);
    assert(state2.getGame().getPieceAt(Square("h4")) == Piece('Q'));

    return 0;
}

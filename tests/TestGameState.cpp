//#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>

#include "../src/GameState.h"

using namespace Chess;

TEST_CASE("board generates FEN, remembers positions", "[board]") {
    Board board;
    std::cout << "Board FEN: " << board.getBoardFen() << std::endl;

    REQUIRE(board.getBoardFen() == Board::initialBoardFen);
    REQUIRE(board.getPieceAt(Square(0, 0)) == Piece(Piece::Type::Rook, Color::White));
    REQUIRE(Square("e7") == Square(6, 4));
    REQUIRE(Piece('p') == Piece(Piece::Type::Pawn, Color::Black));
    REQUIRE(board.getPieceAt(Square("e7")) == Piece('p'));
}

TEST_CASE("game state handles FEN, provides additional state", "[gamestate]") {
    GameState gameState;
    std::cout << "FEN: " << gameState.getFen() << std::endl;
    REQUIRE(gameState.getFen() == GameState::initialFen);
    REQUIRE(gameState.getTurn() == Color::White);
    REQUIRE(gameState.canCastle('K') == true);

    gameState.setFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    REQUIRE(gameState.getPieceAt(Square("e4")) == Piece('P'));
    REQUIRE(gameState.getTurn() == Color::Black);
    REQUIRE(gameState.canCastle(Piece('K')));
    REQUIRE(gameState.canCastle(Piece('Q')));
    REQUIRE(gameState.canCastle(Piece('k')));
    REQUIRE(gameState.canCastle(Piece('q')));
    REQUIRE(gameState.getEnPassant().has_value());
    REQUIRE(gameState.getEnPassant().value() == Square("e3"));
    REQUIRE(gameState.getHalfmoveClock() == 0);
    REQUIRE(gameState.getFullmoveNumber() == 1);
}

TEST_CASE("game provides move history", "[history]") {
    // Test game (complete description of a chess game including move history).
    Game game;
    // Push a move and ensure that the state has updated. (Might be nice to support algebraic notation here.)
    game.push(Move(Square("e2"), Square("e4")));
    REQUIRE(game.getFen() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    REQUIRE(game.getPieceAt(Square("e4")).value() == Piece('P'));
    REQUIRE(!game.getPieceAt(Square("e2")).has_value());
    // Confirm that popping restores previous state.
    REQUIRE(game.pop() == Move(Square("e2"), Square("e4")));
    REQUIRE(game.getFen() == GameState::initialFen);
    // Confirm that history is LIFO and en passant works as expected:
    game.push(Move(Square("d2"), Square("d4")));
    REQUIRE(game.getEnPassant() == Square("d3"));
    game.push(Move(Square("b8"), Square("c6")));
    REQUIRE(game.getEnPassant() == std::nullopt);
    REQUIRE(game.pop() == Move(Square("b8"), Square("c6")));
    REQUIRE(game.pop() == Move(Square("d2"), Square("d4")));

    REQUIRE(game.getFen() == GameState::initialFen);
    REQUIRE(game.isLegal(Move(Square("e2"), Square("e4"))));
    REQUIRE(!game.isLegal(Move(Square("e2"), Square("e5"))));
}

TEST_CASE("castling works", "[castling]") {
    Game game("rnbqkbnr/pp3ppp/2ppp3/8/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");
    Move move(Square("e1"), Square("g1"));
    REQUIRE(game.isLegal(move));
    game.push(move);
    REQUIRE(!game.canCastle(Piece('K')));
    REQUIRE(!game.canCastle(Piece('K')));
    REQUIRE(game.getPieceAt(Square("f1")) == Piece('R'));
    REQUIRE(game.getPieceAt(Square("g1")) == Piece('K'));
    REQUIRE(!game.getPieceAt(Square("e1")));
    REQUIRE(!game.getPieceAt(Square("h1")));
}

TEST_CASE("AppState singleton provides access to game", "[singleton]") {
    // Ensure singleton works as expected.
    AppState &state = AppState::getInstance();
    state.getGame().setPieceAt(Square("h4"), Piece('Q'));

    AppState &state2 = AppState::getInstance();
    REQUIRE(&state == &state2);
    REQUIRE(state2.getGame().getPieceAt(Square("h4")) == Piece('Q'));
}

TEST_CASE("detects checkmate", "[outcomes]") {

    GameState gameState;
    gameState.setFen("r2q1k1r/ppp1bQ2/2npBp1p/4pPp1/4P3/3P3P/PPP3P1/R1B2RK1 b - - 3 15");
    REQUIRE(gameState.isInCheck(Color::Black));
    std::optional<std::optional<Color>> color = gameState.getOutcome();
    REQUIRE(color == Color::White);
}

TEST_CASE("detects stalemate", "[outcomes]") {

    GameState gameState;
    gameState.setFen("k7/P7/1Q6/3K4/8/8/8/8 b - - 0 1");
    std::optional<std::optional<Color>> color = gameState.getOutcome();
    REQUIRE(color == std::make_optional<std::optional<Color>>(std::nullopt));
}


TEST_CASE("detects undetermined outcome", "[outcomes]") {

    GameState gameState;
    std::optional<std::optional<Color>> color = gameState.getOutcome();
    REQUIRE(color == std::nullopt);
}

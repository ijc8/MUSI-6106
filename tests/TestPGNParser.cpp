
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include "../src/PGNParser.h"
#include "../src/GameState.h"

using namespace Chess;

TEST_CASE("Reading PGN tags", "[PGN]") {

    std::unordered_map<std::string, std::string> tags;
    PGNParser pgn;
    tags = pgn.extractTags();

}


TEST_CASE("Reading PGN moves (algebraic)", "[PGN]") {

    std::vector<std::string> moves;
    PGNParser pgn;
    moves = pgn.getMovesAlgebraic();

}

TEST_CASE("Reading PGN moves (on the board)", "[PGN]") {

    std::vector<std::string> moves;
    PGNParser pgn;
    moves = pgn.getMovesAlgebraic();
    

}
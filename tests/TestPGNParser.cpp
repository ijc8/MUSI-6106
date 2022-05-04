
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

    std::string testPGN = R"(1. e4 c5 2. Nf3 Nc6 3. d4 cxd4 4. Nxd4 g6 5. c4 Nf6 6. Nc3 d6 7. f3 Nxd4 8. Qxd4
Bg7 9. Be3 O-O 10. Qd2 Be6 11. Rc1 Qa5 12. Nd5 Qxd2+ 13. Kxd2 Bxd5 14. cxd5 Rfc8
15. Rxc8+ Rxc8 16. g3 Nd7 17. Bh3 Rc7 18. Bxd7 Rxd7 19. b3 a6 20. Rc1 f5 21.
Rc8+ Kf7 22. Kd3 e6 23. Kc4 fxe4 24. fxe4 Re7 25. Bf4 b5+ 26. Kd3 exd5 27. exd5
Re1 28. Rc7+ Kg8 29. Rc6 Rd1+ 30. Ke4 Rd4+ 31. Ke3 Rxd5 32. Rxa6 Bf8 33. Ke4 Rd1
34. Ra5 b4 35. Rd5 Re1+ 36. Kd3 Kf7 37. Ra5 Ke6 38. Ra4 Rd1+ 39. Ke2 Rh1 40. h4
Rh2+ 41. Kd3 Be7 42. Ra6 Kd5 43. Ra7 Ke6 44. Kc4 d5+ 45. Kd4 Bf6+ 46. Kd3 h5 47.
Ra6+ Kf7 48. Ke3 Rc2 49. Bd6 Bc3 50. Bc5 Rg2 51. Kf3 Rc2 52. Kf4 Bd2+ 53. Be3
Bc3 54. Kg5 d4 55. Bf4 d3 56. Rxg6 Rxa2 57. Rd6 Ra5+ 58. Kh6 Bg7+ 59. Kh7 Bc3
60. Rxd3 Ra8 61. Kh6 Ra5 62. Bg5 Ra3 63. Kxh5 Rxb3 64. Re3 Ra3 65. Re7+ Kf8 66.
Rb7 b3 67. Be7+ 1-0)";
    std::vector<std::string> moves;
    PGNParser pgn;
    moves = pgn.getMovesAlgebraic(testPGN);

}

TEST_CASE("Reading PGN moves (on the board)", "[PGN]") {

    Game game;
    std::string SANMove = "e4";
    PGNParser pgn;
    Chess::Move move = PGNParser::placeMovesOnBoard(game, SANMove);
    std::cout<<move.toString();

}
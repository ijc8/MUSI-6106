#include "../src/GameState.h"
#include "../src/EngineBridge.h"

// TODO: Put this in a test.
// int main() {
//     ChessEngine engine;
//     std::cout << engine.getMove().toString() << std::endl;
//     std::cout << engine.getMove(100).toString() << std::endl;
//     return 0;
// }

using namespace Chess;

int main(int argc, const char **argv) {
    Chess::Engine engine("/usr/games/stockfish");
    Game game(argc > 1 ? argv[1] : Game::initialFen);

    bool playing = true;
    while (playing) {
        game.print();
        while (true) {
            std::string moveString;
            std::cout << "> ";
            std::cin >> moveString;
            if (moveString.empty()) {
                return 0;
            }
            Move move(moveString);
            if (game.isLegal(move)) {
                game.push(move);
                break;
            } else {
                std::cout << "Illegal move!" << std::endl;
            }
        }
        game.print();
        std::cout << "Engine is deciding on a move..." << std::endl;
        Chess::Analysis analysis = engine.analyze(game);
        std::cout << "< " << analysis.bestMove.toString() << " - evaluation (centipawns): " << analysis.score << std::endl;
        assert(game.isLegal(analysis.bestMove));
        game.push(analysis.bestMove);
    }

    return 0;
}

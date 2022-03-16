#include "GameState.h"
#include "EngineBridge.h"

// TODO: Put this in a test.
// int main() {
//     Stockfish engine;
//     std::cout << engine.getMove().toString() << std::endl;
//     std::cout << engine.getMove(100).toString() << std::endl;
//     return 0;
// }

using namespace Chess;

int main(int argc, const char **argv) {
    Stockfish engine;
    Game game(argc > 1 ? argv[1] : Game::initialFen);

    auto printBoard = [&game](){
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << "12345678"[rank] << "|";
            for (int file = 0; file < 8; file++) {
                std::optional<Piece> p = game.getPieceAt(Square(rank, file));
                std::cout << (p.has_value() ? p->toChar() : ' ') << "|";
            }
            std::cout << std::endl;
        }
        std::cout << "  ";
        for (int file = 0; file < 8; file++) {
            std::cout << (char)('a' + file) << " ";
        }
        std::cout << std::endl;
        std::cout << "FEN: " << game.getFen() << std::endl;
    };

    bool playing = true;
    while (playing) {
        printBoard();
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
        printBoard();
        std::cout << "Engine is deciding on a move..." << std::endl;
        engine.setState(game);
        Move engineMove = engine.getMove();
        std::cout << "< " << engineMove.toString() << std::endl;
        assert(game.isLegal(engineMove));
        game.push(engineMove);
    }

    return 0;
}

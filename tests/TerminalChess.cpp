#include <iostream>

#include "../src/GameState.h"

using namespace Chess;

int main(int argc, const char **argv) {
    Game game(argc > 1 ? argv[1] : Game::initialFen);
    bool playing = true;
    while (playing) {
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
        while (true) {
            std::string s1, s2;
            std::cout << "> ";
            std::cin >> s1 >> s2;
            if (s1.empty() || s2.empty()) {
                playing = false;
                break;
            }
            Square src(s1), dst(s2);
            Move move(src, dst);
            if (game.isLegal(move)) {
                game.push(move);
                break;
            } else {
                std::cout << "Illegal move!" << std::endl;
            }
        }
    }
}

#include <iostream>

#include "catch.hpp"

#include "../src/GameState.h"
#include "../src/GameStream.h"

int main() {
    std::cout << "Enter game ID: ";
    std::string id;
    std::cin >> id;
    Chess::GameState game;
    GameStream stream(id);
    while (!stream.finished()) {
        std::optional<Chess::Move> move;
        while (move = stream.pollMove()) {
            std::cout << "Got move: " << move->toString() << std::endl;
            game.execute(*move);

            // TODO: Don't duplicate this across test targets.
            for (int rank = 7; rank >= 0; rank--) {
                std::cout << "12345678"[rank] << "|";
                for (int file = 0; file < 8; file++) {
                    std::optional<Chess::Piece> p = game.getPieceAt(Chess::Square(rank, file));
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
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

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
            game.print();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

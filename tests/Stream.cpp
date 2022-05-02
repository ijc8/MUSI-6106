#include <iostream>

#include "catch.hpp"

#include "../src/GameState.h"
#include "../src/GameStream.h"

int main() {
    std::cout << "Enter game ID: ";
    std::string id;
    std::cin >> id;
    Chess::GameState game;
    std::cout << "Press any key to stop streaming." << std::endl;
    GameStream stream(id, [&game](std::optional<Chess::Move> move) {
        if (!move) return;
        std::cout << "Got move: " << move->toString() << std::endl;
        game.execute(*move);
        game.print();
    });
    std::string line;
    std::getline(std::cin, line);
    std::getline(std::cin, line);
    if (!stream.finished()) stream.cancel();
    std::cout << "Exiting gracefully." << std::endl;
}

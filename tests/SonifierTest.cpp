//
// Created by Rose Sun on 2/21/22.
//

#include <iostream>
#include "../src/DebugSonifier.h"
#include "../src/GameState.h"

int main() {

    DebugSonifier testSonifier;
    for (int i=0; i<10; i++){
        std::cout << testSonifier.process() << std::endl;
    }
    Chess::Board board;
    testSonifier.onMove(board);
    for (int i=0; i<10; i++){
        std::cout << testSonifier.process() << std::endl;
    }
    board.setBoardFen("8/8/8/8/8/8/8/8");
    testSonifier.onMove(board);
    for (int i=0; i<10; i++){
        std::cout << testSonifier.process() << std::endl;
    }
    return 0;
}
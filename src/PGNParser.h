//
// Created by Nikhil Ravikrishnan on 3/15/22.
//

#ifndef MUSICAL_CHESS_PGNPARSER_H
#define MUSICAL_CHESS_PGNPARSER_H

#endif //MUSICAL_CHESS_PGNPARSER_H

#include <iostream>
#include "GameState.h"
#include <fstream>
#include <regex>

#define TAGSTART '['
#define TAGEND ']'
#define VALSTART '"'
#define VALEND '"'



using namespace Chess;

struct PGNMove {
    Piece piece;
    Square moveTo;
    Square moveFrom;
    bool isCastling;
    bool isCapture;
    bool isEndOfGame;
    Color color;
};

//static const std::unordered_map<char, Piece> PGNCharToPiece;

class PGNParser : public Chess::GameState {


public:

    PGNParser();
    void setupFileFromPath(std::string path);
    std::unordered_map<std::string, std::string> extractTags();
    std::vector<std::string> getMovesAlgebraic();
    std::unordered_map<int,std::string> getComments();
    std::vector<Chess::Move> getMoves(std::vector<std::string> moves);
    static Chess::Move placeMovesOnBoard(Chess::Game &game,std::vector<std::string> moves);





private:

//    Board m_board;
//    GameState m_State;
    std::ifstream m_PGNFile;
};
//
// Created by Nikhil Ravikrishnan on 3/15/22.
//

#ifndef MUSICAL_CHESS_PGNPARSER_H
#define MUSICAL_CHESS_PGNPARSER_H

#endif //MUSICAL_CHESS_PGNPARSER_H

#include "GameState.h"
#include <fstream>

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
    bool placeMovesOnBoard(std::vector<std::string> moves);




private:

//    Board m_board;
//    GameState m_State;
    std::ifstream m_PGNFile;
};
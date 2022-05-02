//
// Created by Nikhil Ravikrishnan on 3/15/22.
//

#include "PGNParser.h"
#include "../src/GameState.h"
#include <string>
#include <locale>
#include <vector>

using namespace Chess;


const std::unordered_map<char, Piece::Type> PGNCharToPiece{
        {' ', Piece::Type::Pawn},
        {'B', Piece::Type::Bishop},
        {'N', Piece::Type::Knight},
        {'R', Piece::Type::Rook},
        {'Q', Piece::Type::Queen},
        {'K', Piece::Type::King},
};

//const std::unordered_map<char, Piece::Type> PGNSpecialCharacters{
//        {'x', },
//        {'+', },
//        {'O-O', },
//        {'O-O-O', },
//        {'1-0', },
//        {'0-1', },
//        {'1-0', }
//};



PGNParser::PGNParser() {

    setupFileFromPath("/Users/nikhilravikrishnan/Spring2022/ASE/MUSI-6106/tests/test.pgn");
}

void PGNParser::setupFileFromPath(std::string path) {

    m_PGNFile.open(path);
}

/*
 * Function to get the metadata in the tags of the PGN file.
 * Parameters: None
 * Returns: unordered_map <tag, value>
 */

std::unordered_map<std::string, std::string> PGNParser::extractTags() {

    int tagStartIdx = 0;
    int tagEndIdx = 0;

    int valueStartIdx;
    int valueEndIdx;

    std::unordered_map<std::string, std::string> tags;

    std::string line;
    while(std::getline(m_PGNFile, line))
    {
        if(line[0] != TAGSTART)
            break;
        tagStartIdx = 1;
        tagEndIdx = line.find(" ") - 1 ;

        valueStartIdx = line.find( VALSTART);
        valueEndIdx = line.find(VALEND, valueStartIdx + 1);

        tags.insert(std::pair<std::string, std::string>(line.substr(tagStartIdx, tagEndIdx),line.substr(valueStartIdx, valueEndIdx-valueStartIdx + 1)));

    }

    return tags;
}

std::vector<std::string> PGNParser::getMovesAlgebraic() {

    std::locale loc;
    std::vector<std::string> moves;

    int startIdx = 2;
    int endIdx = 0;

    int moveNumber = 1;
    int lineNumber = 0;

    std::string line;
    std::string move;

    while (std::getline(m_PGNFile, line)) {

        if (line[0] == TAGSTART || line.length() == 0)
            continue;

        // Check whether line stars with number or not and set start-end idx
        if (!std::isdigit(line[0], loc)) {
            startIdx = 0;
            endIdx = line.find(std::to_string(moveNumber + 1) + ".");

            if(line.back() == '.') {

            }

            move = move + " " + line.substr(startIdx, endIdx - startIdx);
            moves.push_back(move);
            startIdx = line.find(" ", endIdx);
            moveNumber++;
        }

        if (std::isdigit(line[0], loc) && moves.size() != 0)
        {
            startIdx = line.find(" ");
            endIdx = 0;
            moveNumber++;
        }

        // When the line starts with a number
        while (endIdx != -1) {
            endIdx = line.find(std::to_string(moveNumber + 1) + ".");
            if (endIdx == -1) {
                if(startIdx == -1) {
                    move = " ";
                    break;
                }
                move = line.substr(startIdx, line.length() - startIdx);
                if(move.find("1-0") != -1 || move.find("0-1") != -1 || move.find("1/2-1/2") != -1 )
                    moves.push_back(move);
                break;
            }
            move = line.substr(startIdx, endIdx - startIdx);
            moves.push_back(move);
            startIdx = line.find(" ", endIdx);
            moveNumber++;
        }
    }

    return moves;
}

std::vector<Chess::Move> PGNParser::getMoves(std::vector<std::string> moves) {



    return std::vector<Chess::Move>();
}












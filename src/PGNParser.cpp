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
    while (std::getline(m_PGNFile, line)) {
        if (line[0] != TAGSTART)
            break;
        tagStartIdx = 1;
        tagEndIdx = line.find(" ") - 1;

        valueStartIdx = line.find(VALSTART);
        valueEndIdx = line.find(VALEND, valueStartIdx + 1);

        tags.insert(std::pair<std::string, std::string>(line.substr(tagStartIdx, tagEndIdx),
                                                        line.substr(valueStartIdx, valueEndIdx - valueStartIdx + 1)));

    }

    return tags;
}

std::vector<std::string> PGNParser::getMovesAlgebraic() {


//
    std::string line;
    std::string move;

    std::vector<std::string> moves;
    std::regex matchStr(
            "(?:[PNBRQK]?[a-h]?[1-8]?x?[a-h][1-8](?:\\=[PNBRQK])?|O(-?O){1,2})[\\+#]?(\\s*[\\!\\?]+)? (1\\-0|0\\-1|1\\/2\\-1\\/2)?");
//    std::regex matchStr ("");
    std::smatch sm;
    std::string test = "Qxd4";


    while (std::regex_search(test, sm, matchStr)) {
        std::cout << sm[0] << std::endl;
        moves.push_back(sm[0]);
        test = sm.suffix();

    }

    return moves;
}

std::vector<Chess::Move> PGNParser::getMoves(std::vector<std::string> moves) {


    return std::vector<Chess::Move>();
}

Chess::Move PGNParser::placeMovesOnBoard(Game &game, const std::string &SANMove) {
    Piece piece;
    std::smatch sm;

    std::regex castlingRegex("O(-O){1,2}[\\+#]?");
    std::regex_match(SANMove, sm, castlingRegex);

    // Castling
    if (sm[0].matched) {
        auto turn = game.getTurn();
        if (sm[0].length() >= 5) {
            return Chess::Move(turn == Chess::Color::White ? "e1c1" : "e8c8");
        } else {
            return Chess::Move(turn == Chess::Color::White ? "e1g1" : "e8g8");
        }
    }

    std::regex regexMove("([NBKRQ])?([a-h])?([1-8])?[\\-x]?([a-h][1-8])(=?[nbrqkNBRQK])?[\\+#]?");
    std::regex_match(SANMove, sm, regexMove);

    piece.type = sm[1].length() == 0 ? Piece::Type::Pawn : Piece::FromChar.at(sm[1].str()[0]);
    piece.color = game.getTurn();


    Square toSquare(sm[4].str());

    // Some ASCII stuff to find rank and file of source
    std::optional<int> file = sm[2].length() ? std::make_optional(sm[2].str()[0] - 'a') : std::nullopt;
    std::optional<int> rank = sm[3].length() ? std::make_optional(sm[3].str()[0] - '1') : std::nullopt;

    // Piece Promotion check
    std::optional<Piece::Type> promotion;
    if (sm[5].length() > 0) {
        promotion = Piece::FromChar.at(sm[5].str()[1]);
    }

    for (auto[square, pieceCandidate]: game.getPieceMap()) {
        // Filters to find the right piece and make the move
        if (piece.color != game.getTurn()) continue;
        if (pieceCandidate.type != piece.type) continue;
        if (rank.has_value() && square.rank != rank) continue;
        if (file.has_value() && square.file != file) continue;
        for (auto move: game.generateMoves(square)) {
            if (move.dst == toSquare && move.promotion == promotion) {
                return move;
            }
        }
    }

    throw std::runtime_error("Bad SAN");
}

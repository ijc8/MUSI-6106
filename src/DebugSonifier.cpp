//
// Created by Rose Sun on 2/17/22.
//

#include "DebugSonifier.h"

DebugSonifier::DebugSonifier (){
}

DebugSonifier::~DebugSonifier() {
}

void DebugSonifier::sonifyPiece(Chess::Square const& square, Chess::Piece const& piece) {
    float frequencies[8] = {262, 294.8, 327.5, 349.3, 393, 436.7, 491.2, 524};
    float gains[8] = {1, 0.8, 0.6, 0.4, 0.4, 0.6, 0.8, 1};
    int freqIdx = static_cast<int>(square.file);
    int gainIdx = static_cast<int>(square.rank);

    if (piece.color == Chess::Color::Black && piece.type == Chess::Piece::Type::Pawn){
        oscillators.emplace_back(sine, frequencies[freqIdx], gains[gainIdx],44100);
    }
    else if (piece.color == Chess::Color::Black && piece.type != Chess::Piece::Type::Pawn){
        oscillators.emplace_back(sine, frequencies[freqIdx], gains[gainIdx],44100);
    }
    else if (piece.color == Chess::Color::White && piece.type == Chess::Piece::Type::Pawn){
        oscillators.emplace_back(sine, frequencies[freqIdx], gains[gainIdx],44100);
    }
    else if (piece.color == Chess::Color::White && piece.type != Chess::Piece::Type::Pawn) {
        oscillators.emplace_back(sine, frequencies[freqIdx], gains[gainIdx], 44100);
    } else {
        assert(false);
    }
    oscillators.back().noteOn();
}

float DebugSonifier::process(){
    float out = 0;
    for (CWavetableOscillator &osc: oscillators){
        out += osc.process();
    }
    return out/64;
}


Error_t DebugSonifier::onMove(Chess::Board &board) {
    oscillators.clear();
    for( const auto [square, piece] : board.getPieceMap() ) {
        sonifyPiece(square, piece);
    }
    return Error_t::kNoError;
}

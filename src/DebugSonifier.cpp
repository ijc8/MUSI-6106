//
// Created by Rose Sun on 2/17/22.
//

#include "DebugSonifier.h"
#include "Waveform.h"

DebugSonifier::DebugSonifier(float sampleRate) : Sonifier(sampleRate) {
    srand(static_cast<unsigned>(time(0)));
}

DebugSonifier::~DebugSonifier() {

}

void DebugSonifier::sonifyPiece(Chess::Square const& square, Chess::Piece const& piece) {
    float frequencies[8] = {262.0f, 294.8f, 327.5f, 349.3f, 393.0f, 436.7f, 491.2f, 524.0f};
    float gains[8] = {1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f};
    float pans[8] = { 0.0f, 0.2f, 0.4f, 0.5f, 0.5f, 0.6f, 0.8f, 1.0f };
    int freqIdx = static_cast<int>(square.file);
    int gainIdx = static_cast<int>(square.rank);
    int panIdx = rand() % 8;

    auto inst = std::make_shared<CWavetableOscillator>(Waveform::sine, frequencies[freqIdx], gains[gainIdx] / 32.0, 44100);
    inst->setADSRParameters(2, 0, 1, 2);
    inst->noteOn();
    inst->setPan(pans[panIdx]);
    mMainProcessor.addInst(inst);
    oscillatorPtrs.push_back(inst);
}

void DebugSonifier::onMove(Chess::Game &board) {
    auto it = oscillatorPtrs.begin();
    while (it != oscillatorPtrs.end()){
        if ((*it)->isActive()) {
            (*it)->noteOff();
            it++;
        }
        else {
            mMainProcessor.removeInst(*it);
            it = oscillatorPtrs.erase(it);
        }
    }
    for( const auto [square, piece] : board.getPieceMap() ) {
        sonifyPiece(square, piece);
    }
}

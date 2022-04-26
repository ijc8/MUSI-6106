//
// Created by Rose Sun on 3/7/22.
//

#include "ThreatsSonifier.h"

ThreatsSonifier::ThreatsSonifier (){
    srand(static_cast<unsigned>(time(0)));
}

ThreatsSonifier::~ThreatsSonifier() {
}

void ThreatsSonifier::sonifyThreats(Chess::Square const& preySquare, const std::optional<Piece>& preyPiece)

{
    pan = preySquare.rank + 1 * 0.1f;

    float hitchcockFrequencies[4] = {261.6256f, 311.1270f,391.9954f, 493.8833f};
    float gains[8] = {1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f};
    int gainIdx = static_cast<int>(preySquare.file);

    std::shared_ptr<CInstrument> inst;
    if (preyPiece->type == Chess::Piece::Type::Pawn) {
        inst = std::make_shared<CWavetableOscillator>(saw, hitchcockFrequencies[0], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::Bishop || preyPiece->type == Chess::Piece::Type::Knight || preyPiece->type == Chess::Piece::Type::Rook){
        inst = std::make_shared<CWavetableOscillator>(saw, hitchcockFrequencies[1], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::Queen){
        inst = std::make_shared<CWavetableOscillator>(saw, hitchcockFrequencies[2], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::King){
        inst = std::make_shared<CWavetableOscillator>(saw, hitchcockFrequencies[3], gains[gainIdx], 44100);
    }

    inst->noteOn();
    inst->setADSRParameters(2,0,1,2);
    inst->setPan(pan);
    mMainProcessor.addInst(inst);
    oscillatorPtrs.push_back(inst);

}


void ThreatsSonifier::prepareToPlay(int iExpectedBlockSize, float fSampleRate)
{
    Sonifier::prepareToPlay(iExpectedBlockSize, fSampleRate);
    mMainProcessor.setGain(0.25);

};

void ThreatsSonifier::onMove(Chess::Game &gameState) {
    auto it = oscillatorPtrs.begin();
    while (it != oscillatorPtrs.end()){
        if ((*it)->isActive()){
            (*it)->noteOff();
            it++;
        }
        else {
            mMainProcessor.removeInst(*it);
            it = oscillatorPtrs.erase(it);
        }
    }
    for (const auto [preySquare, preyPiece] : gameState.getThreats() ) {
        sonifyThreats(preySquare, preyPiece);
    }
}

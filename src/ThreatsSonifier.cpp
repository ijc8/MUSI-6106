//
// Created by Rose Sun on 3/7/22.
//

#include "ThreatsSonifier.h"

ThreatsSonifier::ThreatsSonifier (){
    srand(static_cast<unsigned>(time(0)));
}

ThreatsSonifier::~ThreatsSonifier() {
}

void ThreatsSonifier::sonifyThreatee(Chess::Square const& preySquare, const std::optional<Piece>& preyPiece)

{
    pan = preySquare.rank + 1 * 0.1f;

    float hitchcockFrequencies[4] = {261.6256f, 311.1270f,391.9954f, 493.8833f};
    float gains[8] = {1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f};
    int gainIdx = static_cast<int>(preySquare.file);

    if (preyPiece->type == Chess::Piece::Type::Pawn) {
        oscillators.emplace_back(saw, hitchcockFrequencies[0], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::Bishop || preyPiece->type == Chess::Piece::Type::Knight || preyPiece->type == Chess::Piece::Type::Rook){
        oscillators.emplace_back(saw, hitchcockFrequencies[1], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::Queen){
        oscillators.emplace_back(saw, hitchcockFrequencies[2], gains[gainIdx], 44100);
    }

    else if (preyPiece->type == Chess::Piece::Type::King){
        oscillators.emplace_back(saw, hitchcockFrequencies[3], gains[gainIdx], 44100);
    }

    oscillators.back().noteOn();
    oscillators.back().setADSRParameters(2,0,1,2);
    oscillators.back().setPan(pan);
    m_mainProcessor.addInstRef(oscillators.back());


}

void ThreatsSonifier::process(float **ppfOutBuffer, int iNumChannels, int iNumFrames) {

    m_mainProcessor.process(ppfOutBuffer, iNumChannels, iNumFrames);
}


void ThreatsSonifier::prepareToPlay(int iExpectedBlockSize, double fsampleRate){

    m_fSampleRate = static_cast<float>(fsampleRate);
    m_mainProcessor.setSampleRate(static_cast<float>(fsampleRate));
    m_mainProcessor.setGain(0.25);
    m_mainProcessor.setADSRParameters(4,0,1,2);
    m_fExpectedBlockSize = iExpectedBlockSize;

};


void ThreatsSonifier::releaseResources(){

};


Error_t ThreatsSonifier::onMove(Chess::GameState &gameState) {
    auto it = oscillators.begin();
    while (it != oscillators.end()){
        if (it->isActive()){
            it->noteOff();
            it++;
        }
        else {
            m_mainProcessor.removeInstRef(*it);
            if (!m_mainProcessor.contains(*it))
                it = oscillators.erase(it);
            else
                it++;

        }
    }
    for (const auto [preySquare, preyPiece] : gameState.getThreats() ) {
        sonifyThreatee(preySquare, preyPiece);
    }
    return Error_t::kNoError;
}
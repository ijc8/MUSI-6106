//
// Created by Rose Sun on 2/17/22.
//

#include "DebugSonifier.h"

DebugSonifier::DebugSonifier (){
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

    auto inst = std::make_shared<CWavetableOscillator>(sine, frequencies[freqIdx], gains[gainIdx], 44100);
    inst->setADSRParameters(2, 0, 1, 2);
    inst->noteOn();
    inst->setPan(pans[panIdx]);
    m_mainProcessor.addInst(inst);
    oscillatorPtrs.push_back(inst);
}

void DebugSonifier::process(float **ppfOutBuffer, int iNumChannels, int iNumFrames) {

    m_mainProcessor.process(ppfOutBuffer, iNumChannels, iNumFrames);
}


void DebugSonifier::prepareToPlay(int iExpectedBlockSize, double fsampleRate){

    m_fSampleRate = static_cast<float>(fsampleRate);
    m_mainProcessor.setSampleRate(static_cast<float>(fsampleRate));
    m_mainProcessor.setGain(0.25);
    m_mainProcessor.setADSRParameters(4,0,1,2);
    m_fExpectedBlockSize = iExpectedBlockSize;

};


void DebugSonifier::releaseResources()
{

};


Error_t DebugSonifier::onMove(Chess::Board &board) {
    auto it = oscillatorPtrs.begin();
    while (it != oscillatorPtrs.end()){
        if ((*it)->isActive()) {
            (*it)->noteOff();
            it++;
        }
        else {
            m_mainProcessor.removeInst(*it);
            it = oscillatorPtrs.erase(it);
        }
    }
    for( const auto [square, piece] : board.getPieceMap() ) {
        sonifyPiece(square, piece);
    }
    return Error_t::kNoError;
}

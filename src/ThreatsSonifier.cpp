//
// Created by Rose Sun on 3/7/22.
//

#include "ThreatsSonifier.h"

ThreatsSonifier::ThreatsSonifier (){
    srand(static_cast<unsigned>(time(0)));
}

ThreatsSonifier::~ThreatsSonifier() {
}

void ThreatsSonifier::sonifyThreat(Chess::Square const& predSquare, Chess::Piece const& predPiece, Chess::Square const& preySquare, Chess::Piece const& preyPiece)
{
    pan = preySquare.rank + 1 * 0.1f;

    if (preySquare.file < 4)
    {
        // produce sound directly associated with the threat plays a Hitchcock chord
        // pan left and right as if scared
        oscillators.emplace_back(saw, 261.6256, 0.5, 44100);
        oscillators.emplace_back(saw, 523.2511, 0.5, 44100);
        oscillators.emplace_back(saw, 622.2540, 0.5, 44100);
        oscillators.emplace_back(saw, 783.9909, 0.5, 44100);
    }

    else if (preySquare.file >= 4)
    {
        // produce sound directly associated with the threat plays a Hitchcock chord
        // pan left and right as if scared
        oscillators.emplace_back(saw, 622.2540, 0.5, 44100);
        oscillators.emplace_back(saw, 783.9909, 0.5, 44100);
        oscillators.emplace_back(saw, 987.7666, 0.5, 44100);
        oscillators.emplace_back(saw, 1046.502, 0.5, 44100);
    }

    oscillators.back().noteOn();
    oscillators.back().setADSRParameters(2,0,1,2);
    oscillators.back().setPan(pan);
    m_mainProcessor.addInstRef(oscillators.back());

//    CScheduler* pSchedule = new CScheduler(44100);

//    if (predPiece.type == Chess::Piece::Type::Pawn && preyPiece.type != Chess::Piece::Type::Pawn)
//    {
//        // If player is playing computer and player's pawn is threatening a non-pawn piece,
//        // the pawn gets a little pride theme FFAACCFF/FFAACCGF
//        // If two humans are playing, pawn's pride theme is played in sequence with the threat sound
//        // HOW DO I KNOW WHO THE PLAYER IS PLAYING
//        pSchedule->setADSRParameters(0, 0, 1, 0);
//        pSchedule->pushInst(new CWavetableOscillator(saw, 349.2282, 0.5, 44100), 0.1, 0.5);
//        pSchedule->pushInst(new CWavetableOscillator(saw, 440.0000, 0.5, 44100), 0.1, 0.5);
//        pSchedule->pushInst(new CWavetableOscillator(saw, 523.2511, 0.5, 44100), 0.1, 0.5);
//        pSchedule->pushInst(new CWavetableOscillator(saw, 698.4565, 0.5, 44100), 0.1, 0.5);
//    }

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


Error_t ThreatsSonifier::onMove(Chess::Board &board) {
    auto it = oscillators.begin();
    while (it != oscillators.end()){
        if (it->isActive()){
            it->noteOff();
            it++;
        }
        else {
            m_mainProcessor.removeInstRef(*it);
            it = oscillators.erase(it);
        }
    }
    for (const auto [predSquare, predPiece, preySquare, preyPiece] : board.getThreats() ) {
        sonifyThreat(predSquare, predPiece, preySquare, preyPiece);
    }
    return Error_t::kNoError;
}
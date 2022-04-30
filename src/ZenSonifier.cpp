//
// Created by Rose Sun on 2/17/22.
//

#include "ZenSonifier.h"

ZenSonifier::ZenSonifier (){
    srand(static_cast<unsigned>(time(0)));
}

ZenSonifier::~ZenSonifier() {

}

void ZenSonifier::sonifyPiece(Chess::Square const& square, Chess::Piece const& piece) {

    std::shared_ptr<CInstrument> pieceInst;
    if (piece.type == Chess::Piece::Type::Pawn) {

        auto auxLoop = std::make_shared<CLooper>(mSampleRate);

        auto inst1 = std::make_unique<CWavetableOscillator>(sine, FREQ::noteToFreq(mPawnNotes[square.file]), 0.4, mSampleRate);
        auto inst2 = std::make_unique<CWavetableOscillator>(sine, FREQ::noteToFreq(mPawnNotes[rand() % 8]), mGains[square.rank] / 5.0f, mSampleRate);
        inst1->setADSRParameters(1, 0.4, 0.5, 2);
        inst2->setADSRParameters(1, 0.4, 0.5, 2);

        auxLoop->scheduleInst(std::move(inst1), mOnsets[square.rank], 4);
        auxLoop->scheduleInst(std::move(inst2), mOnsets[square.file], 4);

        pieceInst = auxLoop;

    } else if (piece.type == Chess::Piece::Type::King || piece.type == Chess::Piece::Type::Queen) {
        auto royaltyLoop = std::make_shared<CLooper>(mSampleRate);

        auto inst1 = std::make_unique<CWavetableOscillator>(sine, FREQ::noteToFreq(mRoyaltyNotes[square.file]), 0.5, mSampleRate);
        inst1->setADSRParameters(2, 0, 1, 3);

        royaltyLoop->scheduleInst(std::move(inst1), mOnsets[square.file], 4);

        pieceInst = royaltyLoop;
    } else {
        pieceInst = std::make_shared<CWavetableOscillator>(sine, FREQ::noteToFreq(mAuxiliaryNotes[square.file]), mGains[square.rank] / 4.0f, mSampleRate);
        pieceInst->setADSRParameters(2, 0, 1, 2);
    }

    pieceInst->setPan(mPans[rand() % 8]);
    pieceInst->noteOn();
    mMainProcessor.addInst(pieceInst);
    oscillatorPtrs.push_back(pieceInst);
}


void ZenSonifier::prepareToPlay(int iExpectedBlockSize, float fSampleRate)
{
    Sonifier::prepareToPlay(iExpectedBlockSize, fSampleRate);
    mMainProcessor.setGain(0.25);

}
void ZenSonifier::process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames) {
    Sonifier::process(ppfOutputBuffer, iNumChannels, iNumFrames);
}

void ZenSonifier::onMove(Chess::Game &game) {
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
    for( const auto [square, piece] : game.getPieceMap() ) {
        sonifyPiece(square, piece);
    }
}

//
// Created by Rose Sun on 2/17/22.
//

#include "ZenSonifier.h"

ZenSonifier::ZenSonifier(float fSampleRate) : Sonifier(fSampleRate) {
    srand(static_cast<unsigned>(time(0)));
}

ZenSonifier::~ZenSonifier() {

}

void ZenSonifier::sonifyPiece(Chess::Square const& square, Chess::Piece const& piece) {

    std::shared_ptr<CInstrument> pieceInst;
    if (piece.type == Chess::Piece::Type::Pawn) {

        auto pawnLoop = std::make_shared<CLooper>(mSampleRate);

        auto inst1 = std::make_unique<CWavetableOscillator>(Waveform::sine, FREQ::noteToFreq(mPawnNotes[square.file]), 0.4 / 32.0f, mSampleRate);
        auto inst2 = std::make_unique<CWavetableOscillator>(Waveform::sine, FREQ::noteToFreq(mPawnNotes[rand() % 8]), mGains[square.rank] / 32.0f, mSampleRate);
        inst1->setADSRParameters(1, 0.4, 0.5, 2);
        inst2->setADSRParameters(1, 0.4, 0.5, 2);

        pawnLoop->scheduleInst(std::move(inst1), mOnsets[square.rank], 4);
        pawnLoop->scheduleInst(std::move(inst2), mOnsets[square.file], 4);

        pieceInst = pawnLoop;

    } else if (piece.type == Chess::Piece::Type::King || piece.type == Chess::Piece::Type::Queen) {

        auto royaltyLoop = std::make_shared<CLooper>(mSampleRate);
        auto inst = std::make_unique<CWavetableOscillator>(Waveform::sine, FREQ::noteToFreq(mRoyaltyNotes[square.file]), 0.5f / 32.0f, mSampleRate);
        inst->setADSRParameters(2, 0, 1, 3);

        royaltyLoop->scheduleInst(std::move(inst), mOnsets[square.file], 4);

        pieceInst = royaltyLoop;

    } else {

        pieceInst = std::make_shared<CWavetableOscillator>(Waveform::sine, FREQ::noteToFreq(mAuxiliaryNotes[square.file]), mGains[square.rank] / 32.0f, mSampleRate);
        pieceInst->setADSRParameters(2, 0, 1, 2);

    }

    pieceInst->setPan(mPans[rand() % 8]);
    pieceInst->noteOn();
    mMainProcessor.addInst(pieceInst);
    mInstruments.push_back(pieceInst);
}

void ZenSonifier::onMove(Chess::Game &game) {
    auto it = mInstruments.begin();
    while (it != mInstruments.end()) {
        if ((*it)->isActive()) {
            (*it)->noteOff();
            it++;
        }
        else {
            mMainProcessor.removeInst(*it);
            it = mInstruments.erase(it);
        }
    }
    for( const auto [square, piece] : game.getPieceMap() ) {
        sonifyPiece(square, piece);
    }
}

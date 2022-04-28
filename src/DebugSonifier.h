#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "MainProcessor.h"
#include <vector>
#include <list>
#include "Sonifier.h"


class DebugSonifier : public Sonifier {
public:
        DebugSonifier();

        virtual ~DebugSonifier();

        void prepareToPlay(int iExpectedBlockSize, float fSampleRate) override;

protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);

    void onMove(Chess::Game &board) override;

    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;
};


#endif //MUSI_6106_DEBUGSONIFIER_H

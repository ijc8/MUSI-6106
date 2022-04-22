#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include <vector>
#include <list>
#include "SonifierBase.h"


class DebugSonifier : public SonifierBase {
public:
        DebugSonifier();

        virtual ~DebugSonifier();

        void prepareToPlay(int iExpectedBlockSize, float fSampleRate) override;

        void releaseResources() override;

protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);

    Error_t onMove(Chess::Board& board);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        onMove(AppState::getInstance().getGame());
    }
    
    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;

    CSineWavetable sine;


};


#endif //MUSI_6106_DEBUGSONIFIER_H

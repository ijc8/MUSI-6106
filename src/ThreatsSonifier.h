#ifndef MUSI_6106_THREATSSONIFIER_H
#define MUSI_6106_THREATSSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include <vector>
#include <list>
#include <optional>
#include "Sonifier.h"


using namespace Chess;


class ThreatsSonifier : public Sonifier 
{
public:
    ThreatsSonifier();

    virtual ~ThreatsSonifier();

    void prepareToPlay(int iExpectedBlockSize, float fSampleRate) override;

protected:

    void sonifyThreats(Chess::Square const& preySquare, const std::optional<Piece>& preyPiece);

    void onMove(Chess::Game& gameState) override;

    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;

    CSineWavetable sine;

    CSawWavetable saw;

    float pan;

};


#endif //MUSI_6106_THREATSSONIFIER_H

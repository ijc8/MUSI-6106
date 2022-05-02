#ifndef MUSI_6106_THREATSSONIFIER_H
#define MUSI_6106_THREATSSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "MainProcessor.h"
#include <vector>
#include <list>
#include <optional>
#include "Sonifier.h"

class ThreatsSonifier : public Sonifier 
{
public:
    ThreatsSonifier(float sampleRate);

    virtual ~ThreatsSonifier();

protected:

    void sonifyThreats(Chess::Square const& preySquare, const std::optional<Chess::Piece>& preyPiece);

    void onMove(Chess::Game& gameState) override;

    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;

    float pan;

};


#endif //MUSI_6106_THREATSSONIFIER_H

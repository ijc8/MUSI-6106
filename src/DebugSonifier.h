#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "SoundProcessor.h"
#include "GameState.h"
#include "Wavetable.h"
#include <vector>

class DebugSonifier{
public:
        DebugSonifier();

        virtual ~DebugSonifier();

        float process();

        Error_t onMove(Chess::Board & board);


protected:
    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);
    std::vector<CWavetableOscillator> oscillators;
    CSineWavetable sine;


};


#endif //MUSI_6106_DEBUGSONIFIER_H

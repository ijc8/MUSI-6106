#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include <vector>
#include <list>


class ThreatsSonifier{
public:
    ThreatsSonifier();

    virtual ~ThreatsSonifier();

    void process(float **ppfOutBuffer, int iNumChannels, int iNumFrames);

    void prepareToPlay(int iExpectedBlockSize, double fSampleRate);

    void releaseResources();

    Error_t onMove(Chess::Board & board);


protected:

    void sonifyThreat(Chess::Square const& predSquare, Chess::Piece const& predPiece, Chess::Square const& preySquare, Chess::Piece const& preyPiece);

    std::list<CWavetableOscillator> oscillators;

    CSineWavetable sine;

    CSawWavetable saw;

    CMainProcessor m_mainProcessor;

    float m_fSampleRate = 0;

    int m_fExpectedBlockSize = 0;

    float pan;


};


#endif //MUSI_6106_DEBUGSONIFIER_H
#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include <vector>
#include <list>


class DebugSonifier{
public:
        DebugSonifier();

        virtual ~DebugSonifier();

        void process(float **ppfOutBuffer, int iNumChannels, int iNumFrames);

        void prepareToPlay(int iExpectedBlockSize, double fSampleRate);

        void releaseResources();

        Error_t onMove(Chess::Board & board);


protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);

    std::list<CWavetableOscillator> oscillators;

    CSineWavetable sine;

    CMainProcessor m_mainProcessor;

    float m_fSampleRate = 0;

    float m_fExpectedBlockSize = 0;


};


#endif //MUSI_6106_DEBUGSONIFIER_H

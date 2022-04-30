#ifndef MUSI_6106_ZenSonifier_H
#define MUSI_6106_ZenSonifier_H

#include <cassert>
#include <string>
#include <vector>
#include <list>

#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include "Sonifier.h"
#include "Util.h"

class ZenSonifier : public Sonifier {
public:
        ZenSonifier();
        virtual ~ZenSonifier();

        void prepareToPlay(int iExpectedBlockSize, float fSampleRate) override;
        void process(float **ppfOutputBuffer, int iNumChannels, int iNumFrames) override;

protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);
    void onMove(Chess::Game &game) override;

    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;

    CSineWavetable sine;

    std::string mPawnNotes[8] {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"};
    std::string mRoyaltyNotes[8]{"F2", "A2", "C3", "E3", "G3", "A3", "C4", "E4"};
    std::string mAuxiliaryNotes[8]{"D4", "F4", "A4", "C5", "D5", "F5", "A5", "C6"};

    float mGains[8]  {1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f};
    float mPans[8]   {0.0f, 0.2f, 0.4f, 0.5f, 0.5f, 0.6f, 0.8f, 1.0f};
    float mOnsets[8] {0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75};
};


#endif //MUSI_6106_ZenSonifier_H

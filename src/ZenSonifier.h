#ifndef MUSI_6106_ZenSonifier_H
#define MUSI_6106_ZenSonifier_H

#include <cassert>
#include <string>
#include <vector>
#include <list>

#include "ErrorDef.h"
#include "GameState.h"
#include "Waveform.h"
#include "MainProcessor.h"
#include "Sonifier.h"
#include "Util.h"

class ZenSonifier : public Sonifier {
public:
        ZenSonifier(float fSampleRate);
        virtual ~ZenSonifier();

protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);
    void onMove(Chess::Game &game) override;

    std::list<std::shared_ptr<CInstrument>> mInstruments;

    std::string mPawnNotes[8]{"E2", "C4", "D3", "E4", "G6","C5", "A4", "C5" };
    std::string mRoyaltyNotes[8]{"C3", "F2", "A2", "E3", "C4", "G3", "A3", "E4"};
    std::string mAuxiliaryNotes[8]{"D4", "A5", "F4", "A4", "C5",  "F5", "C6","D5"};

    float mGains[8]  {1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f};
    float mPans[8]   {0.0f, 0.2f, 0.4f, 0.5f, 0.5f, 0.6f, 0.8f, 1.0f};
    float mOnsets[8] {4, 1.0, 0.0, 0.25, 3.0, 1.25, 0.5, 1.75};
};


#endif //MUSI_6106_ZenSonifier_H

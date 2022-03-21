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


using namespace Chess;


class ThreatsSonifier : public juce::ChangeListener {
public:
    ThreatsSonifier();

    virtual ~ThreatsSonifier();

    void process(float **ppfOutBuffer, int iNumChannels, int iNumFrames);

    void prepareToPlay(int iExpectedBlockSize, double fSampleRate);

    void releaseResources();

    Error_t onMove(Chess::GameState &gameState);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        onMove(AppState::getInstance().getGame());
    }

    void enable() { m_mainProcessor.noteOn(); onMove(AppState::getInstance().getGame()); };
    void disable() { m_mainProcessor.noteOff(); };

    void setGain(float fNewGain) { m_mainProcessor.setGain(fNewGain); };


protected:

    void sonifyThreatee(Chess::Square const& preySquare, const std::optional<Piece>& preyPiece);

    std::list<CInstrument*> oscillatorPtrs;

    CSineWavetable sine;

    CSawWavetable saw;

    CMainProcessor m_mainProcessor;

    float m_fSampleRate = 0;

    int m_fExpectedBlockSize = 0;

    float pan;

};


#endif //MUSI_6106_THREATSSONIFIER_H
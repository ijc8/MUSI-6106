#pragma once

#include "MainProcessor.h"
#include "GameState.h"
#include "Util.h"

class Sonifier : public juce::ChangeListener, public juce::ActionListener
{
public:
	Sonifier(float sampleRate) : mSampleRate(sampleRate) {
		mMainProcessor.setSampleRate(mSampleRate);
		mMainProcessor.setADSRParameters(1, 0, 1, 1);
	}
	~Sonifier() {}

	virtual void process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames)
	{
		mMainProcessor.process(ppfOutputBuffer, iNumChannels, iNumFrames);
	}

	void setEnabled(bool shouldEnable)
	{
		if (shouldEnable)
			mMainProcessor.noteOn();
		else
			mMainProcessor.noteOff();
	}

	bool isIdle() const
	{
		return !mMainProcessor.isActive();
	}

	void setGain(float fGain)
	{
		mMainProcessor.setGain(fGain);
	}

	virtual void onMove(Chess::Game &game) = 0;

	virtual void actionListenerCallback(const juce::String &message) override {}

    void changeListenerCallback(juce::ChangeBroadcaster* source) override {
        onMove(AppState::getInstance().getGame());
    }
    

protected:

	CMainProcessor mMainProcessor;
	float mSampleRate;
};

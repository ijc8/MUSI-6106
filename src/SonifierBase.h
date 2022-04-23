#pragma once

#include "Wavetable.h"
#include "MainProcessor.h"
#include "GameState.h"
#include "Util.h"

class SonifierBase : public juce::ChangeListener
{
public:
	SonifierBase() {};
	~SonifierBase() {};

	virtual void prepareToPlay(int iExpectedBlockSize, float fSampleRate)
	{
		mSampleRate = fSampleRate;
		mBlockSize = iExpectedBlockSize;

		mMainProcessor.setSampleRate(mSampleRate);
		mMainProcessor.setADSRParameters(1, 0, 1, 1);
	}

	virtual void releaseResources() = 0;

	void process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames)
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

protected:

	CMainProcessor mMainProcessor;
	float mSampleRate = 48000;
	int mBlockSize = 0;

};
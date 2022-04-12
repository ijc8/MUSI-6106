#include "StorySonifier.h"

StorySonifier::StorySonifier()
{
	King = std::make_shared<CWavetableOscillator>(mSine, 440, 1.0f);
}

StorySonifier::~StorySonifier()
{
}

void StorySonifier::process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames)
{
	mMainProcessor.process(ppfOutputBuffer, iNumChannels, iNumFrames);
}

void StorySonifier::prepareToPlay(int iExpectedBlockSize, int iSamplesRate)
{
	mSampleRate = iSamplesRate;
	mBlockSize = iExpectedBlockSize;

	mMainProcessor.setSampleRate(mSampleRate);
	mMainProcessor.setGain(1);

	King->setSampleRate(mSampleRate);
	mMainProcessor.addInst(King);
}

void StorySonifier::releaseResources()
{
}

Error_t StorySonifier::onMove(Chess::Board& board)
{
	return Error_t();
}

void StorySonifier::setEnabled(bool shouldEnable)
{
	if (shouldEnable)
		mMainProcessor.noteOn();
	else
		mMainProcessor.noteOff();
}

void StorySonifier::setGain(float fGain)
{
	mMainProcessor.setGain(fGain);
}

void StorySonifier::sonifyPiece(Chess::Square const& square, Chess::Piece const& piece)
{

}

void StorySonifier::changeListenerCallback(juce::ChangeBroadcaster* source)
{
}

void StorySonifier::actionListenerCallback(const juce::String& message)
{
	if (message.contains("Select"))
	{
		King->noteOn();
	}
}

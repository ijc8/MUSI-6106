#pragma once

#include "Wavetable.h"
#include "MainProcessor.h"
#include "GameState.h"

class StorySonifier : public juce::ChangeListener, public juce::ActionListener
{
public:
	StorySonifier();
	~StorySonifier();

	void process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames);
	void prepareToPlay(int iExpectedBlockSize, int iSamplesRate);
	void releaseResources();

	Error_t onMove(Chess::Board& board);

	void setEnabled(bool shouldEnable);
	void setGain(float fGain);

private:

	void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void actionListenerCallback(const juce::String& message) override;

	CMainProcessor mMainProcessor;
	CSineWavetable mSine;
	CSawWavetable mSaw;
	CSqrWavetable mSquare;

	enum PieceMelody {
		kKingWhite,
		kQueenWhite,
		kKingBlack,
		kQueenBlack,
		
		kNumPieceMelodies
	};

	CInstrument* mSelectedPiece = 0;
	std::shared_ptr<CInstrument> mPieceMelodies[kNumPieceMelodies]
	{
		std::make_shared<CWavetableOscillator>(mSine, 440, 1), // White King
		std::make_shared<CWavetableOscillator>(mSine, 220, 1), // White Queen
		std::make_shared<CWavetableOscillator>(mSaw, 440, 1),  // Black King
		std::make_shared<CWavetableOscillator>(mSaw, 220, 1)   // Black Queen
	};

	std::map<std::string, PieceMelody> toPieceMelodyIdx
	{
		{"K", kKingWhite},
		{"Q", kQueenWhite},
		{"k", kKingBlack},
		{"q", kQueenBlack}
	};

	float mSampleRate = 1.0f;
	float mBlockSize = 0.0f;

};
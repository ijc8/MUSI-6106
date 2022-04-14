#pragma once

#include "Wavetable.h"
#include "MainProcessor.h"
#include "GameState.h"
#include "Util.h"


class StorySonifier : public juce::ChangeListener, public juce::ActionListener
{
public:
	StorySonifier();
	~StorySonifier();

	void process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames);
	void prepareToPlay(int iExpectedBlockSize, float fSampleRate);
	void releaseResources();

	Error_t onMove(Chess::Board& board);

	void setEnabled(bool shouldEnable);
	void setGain(float fGain);

private:

	void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);
	void initializeMemberInstruments(float fSampleRate);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void actionListenerCallback(const juce::String& message) override;

	CMainProcessor mMainProcessor;
	CSineWavetable mSine;
	CSawWavetable mSaw;
	CSqrWavetable mSquare;

	enum PieceMelody {
		kKingWhite,
		kKingBlack,
		kQueenWhite,
		kQueenBlack,
		kKnightWhite,
		kKnightBlack,
		kBishopWhite,
		kBishopBlack,
		kRookWhite,
		kRookBlack,
		kPawnWhite,
		kPawnBlack,

		
		kNumPieceMelodies
	};

	std::shared_ptr<CInstrument> mPieceMelodies[kNumPieceMelodies]{};
	std::map<std::string, PieceMelody> toPieceMelodyIdx
	{
		{"K", kKingWhite},
		{"k", kKingBlack},
		{"Q", kQueenWhite},
		{"q", kQueenBlack},
		{"N", kKnightWhite},
		{"n", kKnightBlack},
		{"B", kBishopWhite},
		{"b", kBishopBlack},
		{"R", kRookWhite},
		{"r", kRookBlack},
		{"P", kPawnWhite},
		{"p", kPawnBlack}
	};

	std::shared_ptr<CLooper> mBassTriad;
	std::shared_ptr<CLooper> mAccompTriad;
	std::shared_ptr<CLooper> mMelody1;
	std::shared_ptr<CLooper> mMelody2;
	std::shared_ptr<CInstrument> mCheckAlarm;

	int mBoardChangeCounter = 0;
	const float mBpm = 150;
	float mSampleRate = 48000.0f;
	float mBlockSize = 0.0f;

};
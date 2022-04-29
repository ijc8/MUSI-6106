#pragma once

#include "Wavetable.h"
#include "MainProcessor.h"
#include "GameState.h"
#include "Util.h"
#include "EngineBridge.h"


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
	void StringToPitchBeat(char* seq, std::string* pitch, std::string* beat);

	std::string addpitch(std::string str, int number);
	

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

	std::shared_ptr<CLooper> mBass;
	std::shared_ptr<CLooper> mAccomp;
	std::shared_ptr<CLooper> mMelodyL;
	std::shared_ptr<CLooper> mMelodyR;
	std::shared_ptr<CLooper> mMelodyJiarui;
	std::shared_ptr<CInstrument> mCheckAlarm;

	int score_game = 5;



	int mBoardChangeCounter = 0;
	const float mBpm = 150;
	float mSampleRate = 48000.0f;
	float mBlockSize = 0.0f;

};
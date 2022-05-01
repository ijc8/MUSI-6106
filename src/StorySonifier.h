#pragma once

#include "MainProcessor.h"
#include "GameState.h"
#include "Util.h"
#include "Sonifier.h"

class StorySonifier : public Sonifier
{
public:
	StorySonifier(float sampleRate);
	~StorySonifier();

	void onMove(Chess::Game &game) override;

private:

	void initializeMemberInstruments(float fSampleRate);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

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
	std::map<char, PieceMelody> toPieceMelodyIdx
	{
		{'K', kKingWhite},
		{'k', kKingBlack},
		{'Q', kQueenWhite},
        {'q', kQueenBlack},
		{'N', kKnightWhite},
		{'n', kKnightBlack},
		{'B', kBishopWhite},
		{'b', kBishopBlack},
		{'R', kRookWhite},
		{'r', kRookBlack},
		{'P', kPawnWhite},
		{'p', kPawnBlack}
	};

	std::shared_ptr<CLooper> mBass;
	std::shared_ptr<CLooper> mAccomp;
	std::shared_ptr<CLooper> mMelodyL;
	std::shared_ptr<CLooper> mMelodyR;

	int mBoardChangeCounter = 0;
	const float mBpm = 150;

};

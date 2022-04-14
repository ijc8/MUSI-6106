#include "StorySonifier.h"

StorySonifier::StorySonifier()
{

}
 
StorySonifier::~StorySonifier()
{
}

void StorySonifier::process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames)
{
	mMainProcessor.process(ppfOutputBuffer, iNumChannels, iNumFrames);
}

void StorySonifier::prepareToPlay(int iExpectedBlockSize, float fSampleRate)
{
	mSampleRate = fSampleRate;
	mBlockSize = iExpectedBlockSize;

	mMainProcessor.setSampleRate(mSampleRate);
	mMainProcessor.setGain(0.3);

	initializeMemberInstruments(fSampleRate);

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
	mBoardChangeCounter++;
	switch (mBoardChangeCounter)
	{
	case 2:
		mBassTriad->noteOn(false);
		break;
	case 6:
		mAccompTriad->noteOn(false);
		break;
	case 7:
		mMelody1->noteOn(false);
		break;
	case 8:
		mMelody2->noteOn(false);
		break;
	}
}

void StorySonifier::actionListenerCallback(const juce::String& message)
{
	if (message.contains("Select"))
	{
		std::string pieceString = message.substring(7, 8).toStdString();
		auto it = toPieceMelodyIdx.find(pieceString);
		if (it != toPieceMelodyIdx.end())
		{
			mPieceMelodies[it->second]->noteOn();
		}
	}
	else if (message.contains("Deselect"))
	{
		std::string pieceString = message.substring(9, 10).toStdString();
		auto it = toPieceMelodyIdx.find(pieceString);
		if (it != toPieceMelodyIdx.end())
		{
			mPieceMelodies[it->second]->noteOff();
		}
	}
	else if (message.contains("Warn"))
	{
		mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(mSquare, 555, 0.25, mSampleRate), 0, 1);
	}
	else if (message.contains("Encourage"))
	{
		mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 555, 0.25, mSampleRate), 0, 1);
	}
}

void StorySonifier::initializeMemberInstruments(float fSampleRate)
{
	mPieceMelodies[kKingWhite] = std::make_shared<CWavetableOscillator>(mSine, FREQ::noteToFreq("A4"), 1, fSampleRate); // White King
	mPieceMelodies[kKingBlack] = std::make_shared<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A4"), 1, fSampleRate);

	auto QueenWhiteChord = std::make_shared<CScheduler>(fSampleRate);
	QueenWhiteChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1.0, fSampleRate), 0, 1);
	QueenWhiteChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("G#4"), 1.0, fSampleRate), 0, 1);
	QueenWhiteChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("B4"), 1.0, fSampleRate), 0, 1);
	mPieceMelodies[kQueenWhite] = QueenWhiteChord;

	auto QueenBlackChord = std::make_shared<CScheduler>(fSampleRate);
	QueenBlackChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1.0, fSampleRate), 0, 1);
	QueenBlackChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("G#4"), 1.0, fSampleRate), 0, 1);
	QueenBlackChord->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("B4"), 1.0, fSampleRate), 0, 1);
	mPieceMelodies[kQueenBlack] = QueenBlackChord;

	auto KnightWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	KnightWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("A3"), 0.5f, fSampleRate), 0, 0.2);
	KnightWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1.0f, fSampleRate), 0.18, 1);
	mPieceMelodies[kKnightWhite] = KnightWhiteLoop;

	auto KnightBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	KnightBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A3"), 0.5f, fSampleRate), 0, 0.2);
	KnightBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1.0f, fSampleRate), 0.18, 1);
	mPieceMelodies[kKnightBlack] = KnightBlackLoop;

	auto BishopWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	float bishopBpm = 80;
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("D4"), 1.0, fSampleRate), 0, TEMPO::beatToSec(0.25, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1.0, fSampleRate), TEMPO::beatToSec(0.25, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("F4"), 1.0, fSampleRate), TEMPO::beatToSec(0.5, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("G4"), 1.0, fSampleRate), TEMPO::beatToSec(0.75, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1.0, fSampleRate), TEMPO::beatToSec(1, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("C4"), 1.0, fSampleRate), TEMPO::beatToSec(1.5, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("D4"), 1.0, fSampleRate), TEMPO::beatToSec(1.75, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	mPieceMelodies[kBishopWhite] = BishopWhiteLoop;

	auto BishopBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("D4"), 1.0, fSampleRate), 0, TEMPO::beatToSec(0.25, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1.0, fSampleRate), TEMPO::beatToSec(0.25, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("F4"), 1.0, fSampleRate), TEMPO::beatToSec(0.5, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("G4"), 1.0, fSampleRate), TEMPO::beatToSec(0.75, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1.0, fSampleRate), TEMPO::beatToSec(1, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("C4"), 1.0, fSampleRate), TEMPO::beatToSec(1.5, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("D4"), 1.0, fSampleRate), TEMPO::beatToSec(1.75, bishopBpm), TEMPO::beatToSec(0.5, bishopBpm));
	mPieceMelodies[kBishopBlack] = BishopBlackLoop;

	auto RookWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	float rookBpm = 110;
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("B3"), 1, fSampleRate), 0, TEMPO::beatToSec(0.33, rookBpm));
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("C4"), 1, fSampleRate), TEMPO::beatToSec(0.33, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(0.66, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("G4"), 1, fSampleRate), TEMPO::beatToSec(1, rookBpm), TEMPO::beatToSec(0.66, rookBpm));
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(1.66, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("G4"), 1, fSampleRate), TEMPO::beatToSec(2, rookBpm), TEMPO::beatToSec(1, rookBpm));
	mPieceMelodies[kRookWhite] = RookWhiteLoop;

	auto RookBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("B3"), 1, fSampleRate), 0, TEMPO::beatToSec(0.33, rookBpm));
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("C4"), 1, fSampleRate), TEMPO::beatToSec(0.33, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(0.66, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("G4"), 1, fSampleRate), TEMPO::beatToSec(1, rookBpm), TEMPO::beatToSec(0.66, rookBpm));
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(1.66, rookBpm), TEMPO::beatToSec(0.33, rookBpm));
	RookBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("G4"), 1, fSampleRate), TEMPO::beatToSec(2, rookBpm), TEMPO::beatToSec(1, rookBpm));
	mPieceMelodies[kRookBlack] = RookBlackLoop;

	auto PawnWhiteMelody = std::make_shared<CScheduler>(fSampleRate);
	float pawnBpm = 180;
	PawnWhiteMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("C5"), 1, fSampleRate), 0, TEMPO::beatToSec(1, pawnBpm));
	PawnWhiteMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("A4"), 1, fSampleRate), TEMPO::beatToSec(1, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	PawnWhiteMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(1.5, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	PawnWhiteMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E5"), 1, fSampleRate), TEMPO::beatToSec(2, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	mPieceMelodies[kPawnWhite] = PawnWhiteMelody;

	auto PawnBlackMelody = std::make_shared<CScheduler>(fSampleRate);
	PawnBlackMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("C5"), 1, fSampleRate), 0, TEMPO::beatToSec(1, pawnBpm));
	PawnBlackMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A4"), 1, fSampleRate), TEMPO::beatToSec(1, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	PawnBlackMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(1.5, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	PawnBlackMelody->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E5"), 1, fSampleRate), TEMPO::beatToSec(2, pawnBpm), TEMPO::beatToSec(0.5, pawnBpm));
	mPieceMelodies[kPawnBlack] = PawnBlackMelody;

	auto CheckAlarmLoop = std::make_shared<CLooper>(fSampleRate);
	auto AlarmSound = std::make_unique<CWavetableOscillator>(mSquare, 700, 1.0, fSampleRate);
	AlarmSound->setADSRParameters(0.4, 0, 1, 0.4);
	CheckAlarmLoop->scheduleInst(std::move(AlarmSound), 0, 1);
	mCheckAlarm = CheckAlarmLoop;

	for (int i = 0; i < kNumPieceMelodies; i++)
	{
		mMainProcessor.addInst(mPieceMelodies[i]);
	}

	mCheckAlarm->setSampleRate(mSampleRate);

	/////////////////////////////////////////////////////////////////////
	//// Bass Triad
	mBassTriad = std::make_shared<CLooper>(fSampleRate);
	auto bassNote1 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("A1"), 1, fSampleRate);
	bassNote1->setADSRParameters(0.25, 0.1, .8, 0.15);
	bassNote1->setPan(0);
	mBassTriad->scheduleInst(std::move(bassNote1), 0, TEMPO::beatToSec(1, mBpm));

	auto bassNote2 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("C2"), 1, fSampleRate);
	bassNote2->setADSRParameters(0.25, 0.1, .8, 0.15);
	bassNote2->setPan(1);
	mBassTriad->scheduleInst(std::move(bassNote2), TEMPO::beatToSec(1, mBpm), TEMPO::beatToSec(1, mBpm));

	auto bassNote3 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E2"), 1, fSampleRate);
	bassNote3->setADSRParameters(0.25, 0.1, .8, 0.15);
	bassNote3->setPan(0.5);
	mBassTriad->scheduleInst(std::move(bassNote3), TEMPO::beatToSec(2, mBpm), TEMPO::beatToSec(1, mBpm));

	mBassTriad->setADSRParameters(4, 0, 1, 2);
	mMainProcessor.addInst(mBassTriad);
	/////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////
	//// Accomp Triad
	mAccompTriad = std::make_shared<CLooper>(fSampleRate);
	auto accompNote1 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("A4"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote1), 0, TEMPO::beatToSec(12, mBpm));

	auto accompNote2 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("C5"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote2), 0, TEMPO::beatToSec(12, mBpm));

	auto accompNote3 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("E5"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote3), 0, TEMPO::beatToSec(12, mBpm));

	auto accompNote4 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("A4"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote4), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	auto accompNote5 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("D5"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote5), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	auto accompNote6 = std::make_unique<CWavetableOscillator>(mSine, FREQ::noteToFreq("F#5"), 0.5, fSampleRate);
	mAccompTriad->scheduleInst(std::move(accompNote6), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	mMainProcessor.addInst(mAccompTriad);
	mAccompTriad->setGain(0.5);
	mAccompTriad->setADSRParameters(2, 0, 1, 2);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 1
	mMelody1 = std::make_shared<CLooper>(fSampleRate);
	mMelody1->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A5"), 1, fSampleRate), 0, TEMPO::beatToSec(9, mBpm));
	mMelody1->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A5"), 1, fSampleRate), TEMPO::beatToSec(9, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody1->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("B5"), 1, fSampleRate), TEMPO::beatToSec(10, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody1->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("C6"), 1, fSampleRate), TEMPO::beatToSec(11, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody1->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A5"), 1, fSampleRate), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	mMelody1->setADSRParameters(4, 0, 1, 4);
	mMelody1->setGain(0.15);
	mMelody1->setPan(0.25);
	mMainProcessor.addInst(mMelody1);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 2
	mMelody2 = std::make_shared<CLooper>(fSampleRate);
	mMelody2->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1, fSampleRate), 0, TEMPO::beatToSec(9, mBpm));
	mMelody2->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("E4"), 1, fSampleRate), TEMPO::beatToSec(9, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody2->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("G4"), 1, fSampleRate), TEMPO::beatToSec(10, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody2->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("A4"), 1, fSampleRate), TEMPO::beatToSec(11, mBpm), TEMPO::beatToSec(1, mBpm));
	mMelody2->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, FREQ::noteToFreq("F#4"), 1, fSampleRate), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	mMelody2->setADSRParameters(4, 0, 1, 4);
	mMelody2->setGain(0.15);
	mMelody2->setPan(0.75);
	mMainProcessor.addInst(mMelody2);
	/////////////////////////////////////////////////////////////////////
}

#include "StorySonifier.h"

StorySonifier::StorySonifier()
{

}
 
StorySonifier::~StorySonifier()
{
}


void StorySonifier::prepareToPlay(int iExpectedBlockSize, float fSampleRate)
{
	SonifierBase::prepareToPlay(iExpectedBlockSize, fSampleRate);
	mMainProcessor.setGain(1);

	initializeMemberInstruments(fSampleRate);
}

void StorySonifier::releaseResources()
{
}

Error_t StorySonifier::onMove(Chess::Board& board)
{
	return Error_t();
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
		mBass->noteOn(false);
		break;
	case 6:
		mAccomp->noteOn(false);
		break;
	case 7:
		mMelodyL->noteOn(false);
		break;
	case 8:
		mMelodyR->noteOn(false);
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
	auto QueenBlackChord = std::make_shared<CScheduler>(fSampleRate);
	std::vector<std::string> QueenNotes = { "E4", "G#4", "B4" };
	QueenWhiteChord->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), QueenNotes, 1, 110);
	QueenBlackChord->constructChord(CWavetableOscillator(mSaw, 440, 1, fSampleRate), QueenNotes, 1, 110);
	mPieceMelodies[kQueenWhite] = QueenWhiteChord;
	mPieceMelodies[kQueenBlack] = QueenBlackChord;

	auto KnightWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto KnightBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	std::string knightNotes[] = { "A3", "E4" };
	float knightBeats[]		  = { 0.25,  1 };
	KnightWhiteLoop->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), knightNotes, knightBeats, 2, 110);
	KnightBlackLoop->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), knightNotes, knightBeats, 2, 110);
	mPieceMelodies[kKnightWhite] = KnightWhiteLoop;
	mPieceMelodies[kKnightBlack] = KnightBlackLoop;

	auto BishopWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto BishopBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	float bishopBpm = 80;
	std::string bishopNotes[] = { "D4","E4" ,"F4" ,"G4" ,"E4" ,"C4", "D4" };
	float bishopBeats[]		  = { 0.25, 0.25, 0.25, 0.25, 0.5, 0.25, 0.25 };
	BishopWhiteLoop->constructTune(CWavetableOscillator(mSine, 440, 1.0, fSampleRate), bishopNotes, bishopBeats, 7, bishopBpm);
	BishopBlackLoop->constructTune(CWavetableOscillator(mSaw, 44, 1, fSampleRate), bishopNotes, bishopBeats, 7, bishopBpm);
	mPieceMelodies[kBishopWhite] = BishopWhiteLoop;
	mPieceMelodies[kBishopBlack] = BishopBlackLoop;

	auto RookWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto RookBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	float rookBpm = 110;
	std::string rookNotes[] = { "B3", "C4", "E4", "G4", "E4", "G4" };
	float rookBeats[]		= { 0.33, 0.33, 0.33, 0.66, 0.33, 1 };
	RookWhiteLoop->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), rookNotes, rookBeats, 6, rookBpm);
	RookBlackLoop->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), rookNotes, rookBeats, 6, rookBpm);
	mPieceMelodies[kRookWhite] = RookWhiteLoop;
	mPieceMelodies[kRookBlack] = RookBlackLoop;


	auto PawnWhiteMelody = std::make_shared<CScheduler>(fSampleRate);
	auto PawnBlackMelody = std::make_shared<CScheduler>(fSampleRate);
	float pawnBpm = 180;
	std::string pawnNotes[] = {"C5", "A4", "E4", "E5"};
	float pawnBeats[]	    = { 1  , 0.5 , 0.5 , 0.5 };
	PawnWhiteMelody->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, 180);
	PawnBlackMelody->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, 180);
	mPieceMelodies[kPawnWhite] = PawnWhiteMelody;
	mPieceMelodies[kPawnBlack] = PawnBlackMelody;

	auto CheckAlarmLoop = std::make_shared<CLooper>(fSampleRate);
	auto AlarmSound = std::make_unique<CWavetableOscillator>(mSquare, 700, 1.0, fSampleRate);
	AlarmSound->setADSRParameters(0.4, 0, 1, 0.4);
	CheckAlarmLoop->scheduleInst(std::move(AlarmSound), 0, 1);
	mCheckAlarm = CheckAlarmLoop;

	for (int i = 0; i < kNumPieceMelodies; i++)
	{
		mMainProcessor.addInst(mPieceMelodies[i]);
		mPieceMelodies[i]->setGain(0.75);
	}

	mCheckAlarm->setSampleRate(mSampleRate);

	/////////////////////////////////////////////////////////////////////
	//// Bass
	mBass = std::make_shared<CLooper>(fSampleRate);
	std::string bassNotes[] = {"A2", "C3", "E3"};
	float bassBeats[]	    = { 1  , 1   , 1 };
	mBass->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), bassNotes, bassBeats, 3, mBpm);
	mBass->setADSRParameters(4, 0, 1, 2);
	mBass->setGain(0.75);
	mMainProcessor.addInst(mBass);
	/////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////
	//// Accompaniment
	mAccomp = std::make_shared<CLooper>(fSampleRate);

	auto chord1 = std::make_unique<CScheduler>(fSampleRate);
	chord1->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "C5", "E5"}, 12, mBpm);

	auto chord2 = std::make_unique<CScheduler>(fSampleRate);
	chord2->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "D5", "F#5"}, 12, mBpm);

	mAccomp->scheduleInst(std::move(chord1), 0, TEMPO::beatToSec(12, mBpm));
	mAccomp->scheduleInst(std::move(chord2), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	mAccomp->setGain(0.25);
	mAccomp->setADSRParameters(2, 0, 1, 2);
	mMainProcessor.addInst(mAccomp);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 1
	mMelodyL = std::make_shared<CLooper>(fSampleRate);
	std::string melody1Notes[] = {"A5", "A5", "B5", "C6", "A5"};
	float melody1Beats[]	   = { 9  , 1   , 1   , 1   , 12 };
	mMelodyL->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody1Notes, melody1Beats, 5, mBpm);
	mMelodyL->setADSRParameters(4, 0, 1, 4);
	mMelodyL->setGain(0.05);
	mMelodyL->setPan(0.25);
	mMainProcessor.addInst(mMelodyL);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 2
	mMelodyR = std::make_shared<CLooper>(fSampleRate);
	std::string melody2Notes[] = {"E4", "E4", "G4", "A4", "F#4"};
	float melody2Beats[]	   = { 9  , 1   , 1   , 1   ,  12 };
	mMelodyR->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody2Notes, melody2Beats, 5, mBpm);
	mMelodyR->setADSRParameters(4, 0, 1, 4);
	mMelodyR->setGain(0.05);
	mMelodyR->setPan(0.75);
	mMainProcessor.addInst(mMelodyR);
	/////////////////////////////////////////////////////////////////////
}

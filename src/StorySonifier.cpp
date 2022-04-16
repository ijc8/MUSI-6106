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
	auto QueenBlackChord = std::make_shared<CScheduler>(fSampleRate);
	std::vector<std::string> QueenMelody = { "E4", "G#4", "B4" };
	QueenWhiteChord->scheduleChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), QueenMelody, 1, 110);
	QueenBlackChord->scheduleChord(CWavetableOscillator(mSaw, 440, 1, fSampleRate), QueenMelody, 1, 110);
	mPieceMelodies[kQueenWhite] = QueenWhiteChord;
	mPieceMelodies[kQueenBlack] = QueenBlackChord;

	auto KnightWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto KnightBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	std::string knightMelody[] = { "A3", "E4" };
	float knightBeats[]		   = { 0.25,  1 };
	KnightWhiteLoop->scheduleTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), knightMelody, knightBeats, 2, 110);
	KnightBlackLoop->scheduleTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), knightMelody, knightBeats, 2, 110);
	mPieceMelodies[kKnightWhite] = KnightWhiteLoop;
	mPieceMelodies[kKnightBlack] = KnightBlackLoop;

	auto BishopWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto BishopBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	float bishopBpm = 80;
	std::string bishopMelody[] = { "D4","E4","F4","G4","E4","C4","D4" };
	float bishopBeats[]		   = { 0.25, 0.25, 0.25, 0.25, 0.5, 0.25, 0.25 };
	BishopWhiteLoop->scheduleTune(CWavetableOscillator(mSine, 440, 1.0, fSampleRate), bishopMelody, bishopBeats, 7, bishopBpm);
	BishopBlackLoop->scheduleTune(CWavetableOscillator(mSaw, 44, 1, fSampleRate), bishopMelody, bishopBeats, 7, bishopBpm);
	mPieceMelodies[kBishopWhite] = BishopWhiteLoop;
	mPieceMelodies[kBishopBlack] = BishopBlackLoop;

	auto RookWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	auto RookBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	float rookBpm = 110;
	std::string rookMelody[] = { "B3", "C4", "E4", "G4", "E4", "G4" };
	float rookBeats[]		 = { 0.33, 0.33, 0.33, 0.66, 0.33, 1 };
	RookWhiteLoop->scheduleTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), rookMelody, rookBeats, 6, rookBpm);
	RookBlackLoop->scheduleTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), rookMelody, rookBeats, 6, rookBpm);
	mPieceMelodies[kRookWhite] = RookWhiteLoop;
	mPieceMelodies[kRookBlack] = RookBlackLoop;


	auto PawnWhiteMelody = std::make_shared<CScheduler>(fSampleRate);
	auto PawnBlackMelody = std::make_shared<CScheduler>(fSampleRate);
	float pawnBpm = 180;
	std::string pawnNotes[] = {"C5", "A4", "E4", "E5"};
	float pawnBeats[]	    = { 1, 0.5, 0.5, 0.5 };
	PawnWhiteMelody->scheduleTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, 180);
	PawnBlackMelody->scheduleTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, 180);
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
	//// Bass Triad
	mBassTriad = std::make_shared<CLooper>(fSampleRate);
	std::string bassNotes[] = {"A2", "C3", "E3"};
	float bassBeats[] = { 1, 1, 1 };
	mBassTriad->scheduleTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), bassNotes, bassBeats, 3, mBpm);
	mBassTriad->setADSRParameters(4, 0, 1, 2);
	mBassTriad->setGain(0.75);
	mMainProcessor.addInst(mBassTriad);
	/////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////
	//// Accomp Triad
	mAccompTriad = std::make_shared<CLooper>(fSampleRate);

	auto chord1 = std::make_unique<CScheduler>(fSampleRate);
	chord1->scheduleChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "C5", "E5"}, 12, mBpm);

	auto chord2 = std::make_unique<CScheduler>(fSampleRate);
	chord2->scheduleChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "D5", "F#5"}, 12, mBpm);

	mAccompTriad->scheduleInst(std::move(chord1), 0, TEMPO::beatToSec(12, mBpm));
	mAccompTriad->scheduleInst(std::move(chord2), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	mMainProcessor.addInst(mAccompTriad);
	mAccompTriad->setGain(0.25);
	mAccompTriad->setADSRParameters(2, 0, 1, 2);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 1
	mMelody1 = std::make_shared<CLooper>(fSampleRate);
	std::string melody1Notes[] = {"A5", "A5", "B5", "C6", "A5"};
	float melody1Beats[] = { 9, 1, 1, 1, 12 };
	mMelody1->scheduleTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody1Notes, melody1Beats, 5, mBpm);
	mMelody1->setADSRParameters(4, 0, 1, 4);
	mMelody1->setGain(0.05);
	mMelody1->setPan(0.25);
	mMainProcessor.addInst(mMelody1);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 2
	mMelody2 = std::make_shared<CLooper>(fSampleRate);
	std::string melody2Notes[] = {"E4", "E4", "G4", "A4", "F#4"};
	float melody2Beats[] = { 9, 1, 1, 1, 12 };
	mMelody2->scheduleTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody2Notes, melody2Beats, 5, mBpm);
	mMelody2->setADSRParameters(4, 0, 1, 4);
	mMelody2->setGain(0.05);
	mMelody2->setPan(0.75);
	mMainProcessor.addInst(mMelody2);
	/////////////////////////////////////////////////////////////////////
}

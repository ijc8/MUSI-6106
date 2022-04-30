#include "StorySonifier.h"

StorySonifier::StorySonifier()
{

}
 
StorySonifier::~StorySonifier()
{
}


void StorySonifier::prepareToPlay(int iExpectedBlockSize, float fSampleRate)
{
	Sonifier::prepareToPlay(iExpectedBlockSize, fSampleRate);
	mMainProcessor.setGain(1);

	initializeMemberInstruments(fSampleRate);
}

void StorySonifier::onMove(Chess::Game& game) {

	std::optional<Chess::Move> lastMove = game.peek();
    if (lastMove) {
           std::optional<Chess::Piece> movedPiece = game.getPieceAt(lastMove.value().dst);    
		   if (movedPiece) {
               auto it = toPieceMelodyIdx.find(movedPiece.value().toChar());
               if (it != toPieceMelodyIdx.end()) {
                   mPieceMelodies[it->second]->noteOn();
               }
		   }
	}
}

void StorySonifier::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    Sonifier::changeListenerCallback(source);

	mBoardChangeCounter++;
	switch (mBoardChangeCounter)
	{
	case 1:
		mBass->noteOn(false);
		break;
	case 4:
		mAccomp->noteOn(false);
		break;
	case 8:
		mMelodyL->noteOn(false);
		break;
	case 12:
		mMelodyR->noteOn(false);
		break;
	}
}

void StorySonifier::initializeMemberInstruments(float fSampleRate)
{

		auto KingWhiteMelody = std::make_shared<CScheduler>(fSampleRate);
        auto KingBlackMelody = std::make_shared<CScheduler>(fSampleRate);

		auto KingWhiteChord1 = std::make_unique<CScheduler>(fSampleRate);
        auto KingWhiteChord2 = std::make_unique<CScheduler>(fSampleRate);
        auto KingBlackChord1 = std::make_unique<CScheduler>(fSampleRate);
        auto KingBlackChord2 = std::make_unique<CScheduler>(fSampleRate);
        std::vector<std::string> KingNotes{"D4", "F#4", "A4"};
        float kingBpm = 100;
        KingWhiteChord1->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), KingNotes, 0.5, kingBpm);
        KingWhiteChord2->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), KingNotes, 2, kingBpm);
        KingBlackChord1->constructChord(CWavetableOscillator(mSaw, 440, 1, fSampleRate), KingNotes, 0.5, kingBpm);
        KingBlackChord2->constructChord(CWavetableOscillator(mSaw, 440, 1, fSampleRate), KingNotes, 2, kingBpm);

		KingWhiteMelody->scheduleInst(std::move(KingWhiteChord1), 0, TEMPO::beatToSec(0.5, kingBpm));
        KingWhiteMelody->scheduleInst(std::move(KingWhiteChord2), TEMPO::beatToSec(0.5, kingBpm), TEMPO::beatToSec(2, kingBpm));
        KingBlackMelody->scheduleInst(std::move(KingBlackChord1), 0, TEMPO::beatToSec(0.5, kingBpm));
        KingBlackMelody->scheduleInst(std::move(KingBlackChord2), TEMPO::beatToSec(0.5, kingBpm), TEMPO::beatToSec(2, kingBpm));

		mPieceMelodies[kKingWhite] = KingWhiteMelody;
        mPieceMelodies[kKingBlack] = KingBlackMelody;

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
        PawnWhiteMelody->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, pawnBpm);
        PawnBlackMelody->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), pawnNotes, pawnBeats, 4, pawnBpm);
		mPieceMelodies[kPawnWhite] = PawnWhiteMelody;
		mPieceMelodies[kPawnBlack] = PawnBlackMelody;

		for (int i = 0; i < kNumPieceMelodies; i++)
		{
			mMainProcessor.addInst(mPieceMelodies[i]);
			mPieceMelodies[i]->setGain(0.75);
		}


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

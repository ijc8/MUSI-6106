#include "StorySonifier.h"

StorySonifier::StorySonifier()
{
	//score_game = Stockfish::analyze(Chess::GameState & GS, 0)[1];
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
		mBass->noteOn(false);
		mMelodyJiarui->noteOn(false);
		mMelodyL->noteOn(false);
		break;
	//case 6:
	//	mAccomp->noteOn(false);
	//	break;
	//case 7:
	//	mMelodyL->noteOn(false);
	//	break;
	//case 8:
	//	mMelodyR->noteOn(false);
	//	break;
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
	else if (message.contains("Score")) {

		//mMainProcessor.scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 555, 0.25, mSampleRate), 0, 1);
	}
}

void StorySonifier::StringToPitchBeat(char* seq, std::string* pitch, std::string* beat) {

}

std::string StorySonifier::addpitch(std::string str, int number) {
	std::vector<std::string> Notes = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
	char c = 'C';
	int base = (int)c;
	int i = 0;
	if ((!str[0] == 'A') || (!str[0] == 'B')) {
		i = (int)str[0] - base;
		if (str[1] == '#') {
			i++;
			i = i + (int)str[2]*12;
		}
		else {
			i= i + (int)str[1]*12;
		}
	}
	else {
		i = (int)str[0] - base;
		if (str[1] == '#') {
			i++;
			i = i + (int)str[2] * 12+12;
		}
		else {
			i = i + (int)str[1] * 12+12;
		}
	}
	int i2 = i + number;
	int pitch = i2 % 12;
	int number2 = (int)((double)i2/(double)12);
	return Notes[pitch] + (char)(number2-1);

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
	BishopBlackLoop->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), bishopNotes, bishopBeats, 7, bishopBpm);
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
	std::string pawnNotes[] = { addpitch("C5",score_game), addpitch("A4",score_game),addpitch( "E4",score_game), addpitch("E5",score_game) };
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
	//mBass = std::make_shared<CLooper>(fSampleRate);
	//std::string bassNotes[] = {"A2", "C3", "E3"};
	//float bassBeats[]	    = { 1  , 1   , 1 };
	//mBass->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), bassNotes, bassBeats, 3, mBpm);
	//mBass->setADSRParameters(4, 0, 1, 2);
	//mBass->setGain(0.75);
	//mMainProcessor.addInst(mBass);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody_Jiarui1
	mBass = std::make_shared<CLooper>(fSampleRate);
	std::string bassNotes[] = { addpitch("B5",score_game), addpitch("G5",score_game), addpitch("G4",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("G5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("C6",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("G5",score_game), addpitch("A5",score_game), addpitch("B5",score_game), addpitch("G4",score_game), addpitch("G4",score_game), addpitch("A5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("A5",score_game), addpitch("A5",score_game), addpitch("F#5",score_game), addpitch("A4",score_game), addpitch("A5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("B5",score_game), addpitch("D5",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("B4",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("C5",score_game), addpitch("D6",score_game), addpitch("A5",score_game), addpitch("C6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("G5",score_game), addpitch("B4",score_game), addpitch("D5",score_game), addpitch("F#6",score_game), addpitch("F#5",score_game), addpitch("F#6",score_game), addpitch("G5",score_game), addpitch("D6",score_game), addpitch("D6",score_game), addpitch("C6",score_game), addpitch("F#6",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("G5",score_game), addpitch("B5",score_game), addpitch("A5",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("B5",score_game), addpitch("C6",score_game), addpitch("F#6",score_game), addpitch("F#6",score_game), addpitch("D6",score_game), addpitch("D6",score_game), addpitch("G5",score_game), addpitch("D6",score_game), addpitch("A4",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("B4",score_game), addpitch("A5",score_game), addpitch("B5",score_game), addpitch("D6",score_game), addpitch("A4",score_game), addpitch("G5",score_game), addpitch("A5",score_game), addpitch("G5",score_game), addpitch("E6",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("A5",score_game), addpitch("G4",score_game), addpitch("D6",score_game), addpitch("A4",score_game), addpitch("D6",score_game), addpitch("D6",score_game), addpitch("A5",score_game), addpitch("F#6",score_game), addpitch("F#6",score_game), addpitch("G5",score_game), addpitch("B4",score_game), addpitch("B5",score_game), addpitch("G5",score_game), addpitch("G4",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("G5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("C6",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("G5",score_game), addpitch("A5",score_game), addpitch("B5",score_game), addpitch("G4",score_game), addpitch("G4",score_game), addpitch("A5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("A5",score_game), addpitch("A5",score_game), addpitch("F#5",score_game), addpitch("A4",score_game), addpitch("A5",score_game), addpitch("D5",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("B5",score_game), addpitch("D5",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("E6",score_game), addpitch("B4",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("C5",score_game), addpitch("D6",score_game), addpitch("A5",score_game), addpitch("C6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("E6",score_game), addpitch("G5",score_game), addpitch("B4",score_game), addpitch("D5",score_game), addpitch("F#6",score_game), addpitch("F#5",score_game), addpitch("F#6",score_game), addpitch("G5",score_game), addpitch("D6",score_game), addpitch("D6",score_game), addpitch("C6",score_game), addpitch("F#6",score_game), addpitch("D6",score_game), addpitch("B5",score_game), addpitch("G5",score_game) };
	float bassBeats[] = { 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.062, 0.125, 0.125, 0.375, 0.063, 0.062, 0.125, 0.125, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.062, 0.063, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.749, 0.249, 0.25, 0.125, 0.125, 0.249, 0.25, 0.749, 0.25, 0.249, 0.125, 0.125, 0.25, 0.249, 0.875, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.062, 0.125, 0.125, 0.375, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.999, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.062, 0.063, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.062, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.063, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.749, 0.25, 0.25 };
	mBass->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), bassNotes, bassBeats, 152, 23);
	mBass->setADSRParameters(4, 0, 1, 2);
	mBass->setGain(0.30);
	mMainProcessor.addInst(mBass);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody_Jiarui2
	mMelodyJiarui = std::make_shared<CLooper>(fSampleRate);
	std::string melody3Notes[] = { addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("C3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("D3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("G3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game), addpitch("E3",score_game) };
	float melody3Beats[] = { 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.999, 0.999, 0.998, 0.999, 0.999, 0.998, 0.999, 0.999, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125 };
	mMelodyJiarui->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody3Notes, melody3Beats, 76, 23);
	mMelodyJiarui->setADSRParameters(4, 0, 1, 4);
	mMelodyJiarui->setGain(0.65);
	//mMelodyJiarui->setPan(0.75);
	mMainProcessor.addInst(mMelodyJiarui);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody_Jiarui3
	mMelodyL = std::make_shared<CLooper>(fSampleRate);
	std::string melody1Notes[] = { addpitch("E3",score_game), addpitch("E4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("E3",score_game), addpitch("E4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("E3",score_game), addpitch("E4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("E4",score_game), addpitch("E3",score_game), addpitch("E4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("C4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("D4",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("D4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("E3",score_game), addpitch("E4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("E4",score_game), addpitch("E3",score_game), addpitch("E4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("C4",score_game), addpitch("C3",score_game), addpitch("C4",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("D4",score_game), addpitch("D3",score_game), addpitch("D4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("D4",score_game), addpitch("G3",score_game), addpitch("D4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("E3",score_game), addpitch("B4",score_game), addpitch("C4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("C4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("C3",score_game), addpitch("E5",score_game), addpitch("D4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("D4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("D3",score_game), addpitch("A4",score_game), addpitch("D4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("D4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("G3",score_game), addpitch("B4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("E3",score_game), addpitch("B4",score_game), addpitch("C4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("E5",score_game), addpitch("C4",score_game), addpitch("E5",score_game), addpitch("G4",score_game), addpitch("C3",score_game), addpitch("E5",score_game), addpitch("D4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("A4",score_game), addpitch("D4",score_game), addpitch("A4",score_game), addpitch("F#4",score_game), addpitch("D3",score_game), addpitch("A4",score_game), addpitch("D4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("D4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("G3",score_game), addpitch("B4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("B4",score_game), addpitch("E4",score_game), addpitch("B4",score_game), addpitch("G4",score_game), addpitch("E3",score_game), addpitch("B4",score_game) };
	float melody1Beats[]	   = { 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25,0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125 };
	mMelodyL->constructTune(CWavetableOscillator(mSine, 440, 1, fSampleRate), melody1Notes, melody1Beats, 177, 23);
	mMelodyL->setADSRParameters(4, 0, 1, 4);
	mMelodyL->setGain(0.50);
	//mMelodyL->setPan(0.25);
	mMainProcessor.addInst(mMelodyL);
	/////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////
	//// Accompaniment
	//mAccomp = std::make_shared<CLooper>(fSampleRate);

	//auto chord1 = std::make_unique<CScheduler>(fSampleRate);
	//chord1->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "C5", "E5"}, 12, mBpm);

	//auto chord2 = std::make_unique<CScheduler>(fSampleRate);
	//chord2->constructChord(CWavetableOscillator(mSine, 440, 1, fSampleRate), std::vector<std::string>{"A4", "D5", "F#5"}, 12, mBpm);

	//mAccomp->scheduleInst(std::move(chord1), 0, TEMPO::beatToSec(12, mBpm));
	//mAccomp->scheduleInst(std::move(chord2), TEMPO::beatToSec(12, mBpm), TEMPO::beatToSec(12, mBpm));

	//mAccomp->setGain(0.25);
	//mAccomp->setADSRParameters(2, 0, 1, 2);
	//mMainProcessor.addInst(mAccomp);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 1
	//mMelodyL = std::make_shared<CLooper>(fSampleRate);
	//std::string melody1Notes[] = {"A5", "A5", "B5", "C6", "A5"};
	//float melody1Beats[]	   = { 9  , 1   , 1   , 1   , 12 };
	//mMelodyL->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody1Notes, melody1Beats, 5, mBpm);
	//mMelodyL->setADSRParameters(4, 0, 1, 4);
	//mMelodyL->setGain(0.05);
	//mMelodyL->setPan(0.25);
	//mMainProcessor.addInst(mMelodyL);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 2
	//mMelodyR = std::make_shared<CLooper>(fSampleRate);
	//std::string melody2Notes[] = {"E4", "E4", "G4", "A4", "F#4"};
	//float melody2Beats[]	   = { 9  , 1   , 1   , 1   ,  12 };
	//mMelodyR->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody2Notes, melody2Beats, 5, mBpm);
	//mMelodyR->setADSRParameters(4, 0, 1, 4);
	//mMelodyR->setGain(0.05);
	//mMelodyR->setPan(0.75);
	//mMainProcessor.addInst(mMelodyR);
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//// Melody 3
	//mMelodyJiarui = std::make_shared<CLooper>(fSampleRate);
	//std::string melody3Notes[] = { "B5", "G5", "G4", "E6", "E6", "G5", "D5", "B5", "C6", "D6", "B5", "G5", "A5", "B5", "G4", "G4", "A5", "D5", "B5", "A5", "A5", "F#5", "A4", "A5", "D5", "B5", "E6", "B5", "D5", "D6", "B5", "E6", "B4", "E6", "E6", "E6", "C5", "D6", "A5", "C6", "E6", "E6", "E6", "G5", "B4", "D5", "F#6", "F#5", "F#6", "G5", "D6", "D6", "C6", "F#6", "D6", "B5", "G5", "B5", "A5", "E6", "E6", "B5", "C6", "F#6", "F#6", "D6", "D6", "G5", "D6", "A4", "D6", "B5", "E6", "B4", "A5", "B5", "D6", "A4", "G5", "A5", "G5", "E6", "B5", "E6", "A5", "G4", "D6", "A4", "D6", "D6", "A5", "F#6", "F#6", "G5", "B4", "B5", "G5", "G4", "E6", "E6", "G5", "D5", "B5", "C6", "D6", "B5", "G5", "A5", "B5", "G4", "G4", "A5", "D5", "B5", "A5", "A5", "F#5", "A4", "A5", "D5", "B5", "E6", "B5", "D5", "D6", "B5", "E6", "B4", "E6", "E6", "E6", "C5", "D6", "A5", "C6", "E6", "E6", "E6", "G5", "B4", "D5", "F#6", "F#5", "F#6", "G5", "D6", "D6", "C6", "F#6", "D6", "B5", "G5" };
	//float melody3Beats[] = { 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.062, 0.125, 0.125, 0.375, 0.063, 0.062, 0.125, 0.125, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.062, 0.063, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.749, 0.249, 0.25, 0.125, 0.125, 0.249, 0.25, 0.749, 0.25, 0.249, 0.125, 0.125, 0.25, 0.249, 0.875, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.062, 0.125, 0.125, 0.375, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.999, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.062, 0.063, 0.125, 0.125, 0.375, 0.062, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.375, 0.063, 0.062, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.063, 0.063, 0.125, 0.125, 0.375, 0.063, 0.063, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.749, 0.25, 0.25 };
	//mMelodyJiarui->constructTune(CWavetableOscillator(mSaw, 440, 1, fSampleRate), melody2Notes, melody2Beats, 5, 120);
	//mMelodyJiarui->setADSRParameters(4, 0, 1, 4);
	//mMelodyJiarui->setGain(0.05);
	//mMelodyJiarui->setPan(0.75);
	//mMainProcessor.addInst(mMelodyJiarui);
	/////////////////////////////////////////////////////////////////////
}

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
	mPieceMelodies[kKingWhite] = std::make_shared<CWavetableOscillator>(mSine, 440, 1, fSampleRate); // White King
	mPieceMelodies[kKingBlack] = std::make_shared<CWavetableOscillator>(mSaw, 440, 1, fSampleRate);
	mPieceMelodies[kQueenWhite] = std::make_shared<CWavetableOscillator>(mSine, 220, 1, fSampleRate);
	mPieceMelodies[kQueenBlack] = std::make_shared<CWavetableOscillator>(mSaw, 220, 1, fSampleRate);

	auto KnightWhiteLoop = std::make_shared<CLooper>(fSampleRate);
	KnightWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 450, 1.0f, fSampleRate), 0, 1);
	KnightWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 670, 1.0f, fSampleRate), 1, 1);
	mPieceMelodies[kKnightWhite] = KnightWhiteLoop;

	auto KnightBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	KnightBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 670, 1.0f, fSampleRate), 0, 1);
	KnightBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 450, 1.0f, fSampleRate), 1, 1);
	mPieceMelodies[kKnightBlack] = KnightBlackLoop;

	auto BishopWhiteLoop = std::make_shared<CScheduler>(fSampleRate);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 293, 1.0, fSampleRate), 0, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 329, 1.0, fSampleRate), 0.25, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 349, 1.0, fSampleRate), 0.5, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 392, 1.0, fSampleRate), 0.75, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 329, 1.0, fSampleRate), 1.0, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 261, 1.0, fSampleRate), 1.25, 0.25);
	BishopWhiteLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSine, 293, 1.0, fSampleRate), 1.50, 0.5);
	mPieceMelodies[kBishopWhite] = BishopWhiteLoop;

	auto BishopBlackLoop = std::make_shared<CScheduler>(fSampleRate);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 293, 1.0, fSampleRate), 0, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 329, 1.0, fSampleRate), 0.25, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 349, 1.0, fSampleRate), 0.5, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 392, 1.0, fSampleRate), 0.75, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 329, 1.0, fSampleRate), 1.0, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 261, 1.0, fSampleRate), 1.25, 0.25);
	BishopBlackLoop->scheduleInst(std::make_unique<CWavetableOscillator>(mSaw, 293, 1.0, fSampleRate), 1.50, 0.5);
	mPieceMelodies[kBishopBlack] = BishopBlackLoop;

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

	mBassTriad = std::make_shared<CLooper>(fSampleRate);
	auto note1 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][0], 1, fSampleRate);
	note1->setADSRParameters(0.25, 0.1, .8, 0.15);
	note1->setPan(0);
	mBassTriad->scheduleInst(std::move(note1), 0, 1);

	auto note2 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][2], 1, fSampleRate);
	note2->setADSRParameters(0.25, 0.1, .8, 0.15);
	note2->setPan(1);
	mBassTriad->scheduleInst(std::move(note2), 0.5, 1);

	auto note3 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][4], 1, fSampleRate);
	note3->setADSRParameters(0.25, 0.1, .8, 0.15);
	note3->setPan(0.5);
	mBassTriad->scheduleInst(std::move(note3), 1, 0.5);

	mMainProcessor.addInst(mBassTriad);
	mBassTriad->noteOn();

	//mAccompTriad = std::make_shared<CLooper>(fSampleRate);
	//auto note1 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][0], 1, fSampleRate);
	//note1->setADSRParameters(0.25, 0.1, .8, 0.15);
	//note1->setPan(0);
	//mAccompTriad->scheduleInst(std::move(note1), 0, 1);

	//auto note2 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][2], 1, fSampleRate);
	//note2->setADSRParameters(0.25, 0.1, .8, 0.15);
	//note2->setPan(1);
	//mAccompTriad->scheduleInst(std::move(note2), 0.5, 1);

	//auto note3 = std::make_unique<CWavetableOscillator>(mSine, mFrequencies[1][4], 1, fSampleRate);
	//note3->setADSRParameters(0.25, 0.1, .8, 0.15);
	//note3->setPan(0.5);
	//mAccompTriad->scheduleInst(std::move(note3), 1, 0.5);

	//mMainProcessor.addInst(mAccompTriad);
	//mAccompTriad->noteOn(); // TODO: Add option to choose if noteOn resets position
}

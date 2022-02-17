#include "Scheduler.h"

CScheduler::CScheduler(float fSampleRate) :
	CSoundProcessor(fSampleRate)
{
}

CScheduler::~CScheduler()
{
	for (CInstrument* instToDelete : m_InstrumentVector)
		delete instToDelete;
}

float CScheduler::process()
{
	auto startSampleIterator = m_ScheduleStarter.find(iCurrentSample);
	if (startSampleIterator != m_ScheduleStarter.end())
	{
		std::unordered_set<CInstrument*> setToStart = startSampleIterator->second;
		for (CInstrument* instToStart : setToStart)
		{
			instToStart->noteOn();
		}
		m_ScheduleStarter.erase(startSampleIterator);
	}
	auto EndSampleIterator = m_ScheduleEnder.find(iCurrentSample);
	if (EndSampleIterator != m_ScheduleEnder.end())
	{
		std::unordered_set<CInstrument*> setToEnd = EndSampleIterator->second;
		for (CInstrument* instToEnd : setToEnd) 
		{
			instToEnd->noteOff();
		}
		m_ScheduleEnder.erase(EndSampleIterator);
	}

	if (m_ScheduleStarter.empty() && m_ScheduleEnder.empty())
		iCurrentSample = 0;
	else
		iCurrentSample++;

	float fCurrentValue = 0.0f;
	for (CInstrument* instToProcess : m_InstrumentVector)
		fCurrentValue += instToProcess->process();

	return fCurrentValue;
}

Error_t CScheduler::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
		return Error_t::kFunctionInvalidArgsError;

	int iNoteOnInSamp = convertSecToSamp(fOnsetInSec) + iCurrentSample;
	int iNoteOffInSamp = convertSecToSamp(fDurationInSec) + iNoteOnInSamp;
	m_ScheduleStarter[iNoteOnInSamp].insert(pInstrumentToAdd);
	m_ScheduleEnder[iNoteOffInSamp].insert(pInstrumentToAdd);
	m_InstrumentVector.push_back(pInstrumentToAdd);
	return Error_t::kNoError;
}

int CScheduler::convertSecToSamp(float fSec) const
{
	return static_cast<int>(fSec * m_fSampleRateInHz);
}
//==========================================================

//==========================================================
CLooper::CLooper(float fSampleRate) :
	CScheduler(fSampleRate)
{
}

CLooper::~CLooper()
{

}

float CLooper::process()
{
	auto startSampleIterator = m_ScheduleStarter.find(iCurrentSample);
	if (startSampleIterator != m_ScheduleStarter.end())
	{
		std::unordered_set<CInstrument*> setToStart = startSampleIterator->second;
		for (CInstrument* instToStart : setToStart)
		{
			instToStart->noteOn();
		}
	}
	auto EndSampleIterator = m_ScheduleEnder.find(iCurrentSample);
	if (EndSampleIterator != m_ScheduleEnder.end())
	{
		std::unordered_set<CInstrument*> setToEnd = EndSampleIterator->second;
		for (CInstrument* instToEnd : setToEnd)
		{
			instToEnd->noteOff();
		}
	}
	iCurrentSample++;
	iCurrentSample %= iLoopSample;
	float fCurrentValue = 0.0f;
	for (CInstrument* instToProcess : m_InstrumentVector)
		fCurrentValue += instToProcess->process();
	return fCurrentValue;
}

Error_t CLooper::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
		return Error_t::kFunctionInvalidArgsError;

	int iNoteOnInSamp = convertSecToSamp(fOnsetInSec);
	int iNoteOffInSamp = convertSecToSamp(fDurationInSec) + iNoteOnInSamp;
	int iTotalLength = iNoteOffInSamp + pInstrumentToAdd->getADSRParameters().release * m_fSampleRateInHz;
	if (iTotalLength > iLoopSample)
		iLoopSample = iTotalLength;
	m_ScheduleStarter[iNoteOnInSamp].insert(pInstrumentToAdd);
	m_ScheduleEnder[iNoteOffInSamp].insert(pInstrumentToAdd);
	m_InstrumentVector.push_back(pInstrumentToAdd);
	return Error_t::kNoError;

}

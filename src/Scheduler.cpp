#include "Scheduler.h"

CScheduler::CScheduler(float fSampleRate) :
	CSoundProcessor(fSampleRate)
{
}

CScheduler::~CScheduler()
{
	reset();
}

float CScheduler::process()
{
	auto noteOnIterator = m_ScheduleStarter.find(iCurrentSample);
	if (noteOnIterator != m_ScheduleStarter.end())
	{
		std::unordered_set<CInstrument*> setToStart = noteOnIterator->second;
		for (CInstrument* instToStart : setToStart)
		{
			instToStart->noteOn();
		}
		m_ScheduleStarter.erase(noteOnIterator);
	}
	auto noteOffIterator = m_ScheduleEnder.find(iCurrentSample);
	if (noteOffIterator != m_ScheduleEnder.end())
	{
		std::unordered_set<CInstrument*> setToEnd = noteOffIterator->second;
		for (CInstrument* instToEnd : setToEnd) 
		{
			instToEnd->noteOff();
		}
		m_ScheduleEnder.erase(noteOffIterator);
	}
	auto deleteIterator = m_ScheduleDeleter.find(iCurrentSample);
	if (deleteIterator != m_ScheduleDeleter.end())
	{
		std::unordered_set<CInstrument*> setToDelete = deleteIterator->second;
		for (CInstrument* instToDelete : setToDelete)
		{
			m_InstrumentList.remove(instToDelete);
			delete instToDelete;
		}
		m_ScheduleDeleter.erase(deleteIterator);
	}

	if (m_ScheduleStarter.empty() && m_ScheduleEnder.empty() && m_ScheduleDeleter.empty())
		iCurrentSample = 0;
	else
		iCurrentSample++;

	float fCurrentValue = 0.0f;
	for (CInstrument* instToProcess : m_InstrumentList)
		fCurrentValue += instToProcess->process();

	return fCurrentValue;
}

Error_t CScheduler::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
		return Error_t::kFunctionInvalidArgsError;

	int iNoteOnInSamp = convertSecToSamp(fOnsetInSec) + iCurrentSample;
	int iNoteOffInSamp = convertSecToSamp(fDurationInSec) + iNoteOnInSamp;
	int iDeleteInSamp = convertSecToSamp(pInstrumentToAdd->getADSRParameters().release) + iNoteOffInSamp;

	m_ScheduleStarter[iNoteOnInSamp].insert(pInstrumentToAdd);
	m_ScheduleEnder[iNoteOffInSamp].insert(pInstrumentToAdd);
	m_ScheduleDeleter[iDeleteInSamp].insert(pInstrumentToAdd);
	m_InstrumentList.push_front(pInstrumentToAdd);

	return Error_t::kNoError;
}

Error_t CScheduler::reset()
{
	for (CInstrument* instToDelete : m_InstrumentList)
		delete instToDelete;
	m_ScheduleStarter.clear();
	m_ScheduleEnder.clear();
	m_ScheduleDeleter.clear();
	m_InstrumentList.clear();
	iCurrentSample = 0;
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
	reset();
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
	for (CInstrument* instToProcess : m_InstrumentList)
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
	m_InstrumentList.push_front(pInstrumentToAdd);

	return Error_t::kNoError;

}

Error_t CLooper::reset()
{
	iLoopSample = 1;
	return CScheduler::reset();
}

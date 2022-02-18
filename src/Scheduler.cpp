#include "Scheduler.h"

CScheduler::CScheduler(float fSampleRate) :
	CSoundProcessor(fSampleRate)
{
}

CScheduler::~CScheduler()
{
 	reset();
	assert(m_GarbageCollector.empty());
}

float CScheduler::process()
{
	auto noteOnIterator = m_ScheduleNoteOn.find(iCurrentSample);
	if (noteOnIterator != m_ScheduleNoteOn.end())
	{
		std::unordered_set<CInstrument*> setToStart = noteOnIterator->second;
		for (CInstrument* instToStart : setToStart)
		{
			instToStart->noteOn();
		}
		m_ScheduleNoteOn.erase(noteOnIterator);
	}
	auto noteOffIterator = m_ScheduleNoteOff.find(iCurrentSample);
	if (noteOffIterator != m_ScheduleNoteOff.end())
	{
		std::unordered_set<CInstrument*> setToEnd = noteOffIterator->second;
		for (CInstrument* instToEnd : setToEnd) 
		{
			instToEnd->noteOff();
		}
		m_ScheduleNoteOff.erase(noteOffIterator);
	}
	auto removeIterator = m_ScheduleRemover.find(iCurrentSample);
	if (removeIterator != m_ScheduleRemover.end())
	{
		std::unordered_set<CInstrument*> setToRemove = removeIterator->second;
		for (CInstrument* instToRemove : setToRemove)
		{
			m_InstrumentList.remove(instToRemove);
		}
		m_ScheduleRemover.erase(removeIterator);
	}
	iCurrentSample++;

	float fCurrentValue = 0.0f;
	for (CInstrument* instToProcess : m_InstrumentList)
		fCurrentValue += instToProcess->process();

	return fCurrentValue;
}

Error_t CScheduler::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (m_InstrumentList.empty())
		iCurrentSample = 0;
	m_GarbageCollector.push_back(pInstrumentToAdd);
	return addToSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
}

Error_t CScheduler::add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	CInstrument* pInstrumentToAdd = &rInstrumentToAdd;
	return addToSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
}

Error_t CScheduler::reset()
{
	for (CInstrument* instToDelete : m_GarbageCollector)
		delete instToDelete;
	m_ScheduleNoteOn.clear();
	m_ScheduleNoteOff.clear();
	m_ScheduleRemover.clear();
	m_InstrumentList.clear();
	m_GarbageCollector.clear();
	iCurrentSample = 0;
	return Error_t::kNoError;
}

int CScheduler::convertSecToSamp(float fSec) const
{
	return static_cast<int>(fSec * m_fSampleRateInHz);
}
Error_t CScheduler::addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
		return Error_t::kFunctionInvalidArgsError;
	int iNoteOnInSamp = convertSecToSamp(fOnsetInSec);
	int iNoteOffInSamp = iNoteOnInSamp + convertSecToSamp(fDurationInSec - pInstrumentToAdd->getADSRParameters().release);
	assert(iNoteOffInSamp > iNoteOnInSamp);
	if (iNoteOffInSamp < iNoteOnInSamp)
		return Error_t::kFunctionInvalidArgsError;

	m_ScheduleNoteOn[iNoteOnInSamp].insert(pInstrumentToAdd);
	m_ScheduleNoteOff[iNoteOffInSamp].insert(pInstrumentToAdd);
	m_InstrumentList.push_front(pInstrumentToAdd);
	return Error_t::kNoError;
}
Error_t CScheduler::addToInstRemover(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	int iDoneSamp = convertSecToSamp(fOnsetInSec + fDurationInSec);
	m_ScheduleRemover[iDoneSamp].insert(pInstrumentToAdd);
	return Error_t::kNoError;
}
Error_t CScheduler::addToSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (addToADSRSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec) == Error_t::kNoError)
		return addToInstRemover(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
	return Error_t::kFunctionInvalidArgsError;
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
	assert(m_GarbageCollector.empty());
}

float CLooper::process()
{
	auto startSampleIterator = m_ScheduleNoteOn.find(iCurrentSample);
	if (startSampleIterator != m_ScheduleNoteOn.end())
	{
		std::unordered_set<CInstrument*> setToStart = startSampleIterator->second;
		for (CInstrument* instToStart : setToStart)
		{
			instToStart->noteOn();
		}
	}
	auto EndSampleIterator = m_ScheduleNoteOff.find(iCurrentSample);
	if (EndSampleIterator != m_ScheduleNoteOff.end())
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
	if (addToADSRSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec) == Error_t::kNoError)
	{
		int iDoneSamp = convertSecToSamp(fOnsetInSec + fDurationInSec);
		if (iDoneSamp > iLoopSample)
			iLoopSample = iDoneSamp;
		return Error_t::kNoError;
	};
	return Error_t::kFunctionInvalidArgsError;
}

Error_t CLooper::add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	CInstrument* pInstrumentToAdd = &rInstrumentToAdd;
	return add(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
}

Error_t CLooper::reset()
{
	iLoopSample = 1;
	return CScheduler::reset();
}

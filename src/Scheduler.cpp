#include "Scheduler.h"

CScheduler::CScheduler(float fSampleRate) :
	CSoundProcessor(fSampleRate)
{
}

CScheduler::~CScheduler()
{
}

void CScheduler::process()
{
	auto startSampleIterator = m_ScheduleStarter.find(iCurrentSample);
	if (startSampleIterator != m_ScheduleEnder.end())
	{
		std::unordered_set<CInstrument*> setToStart = startSampleIterator->second;
		for (CInstrument* instToStart : setToStart)
			instToStart->noteOn();
	}
	auto EndSampleIterator = m_ScheduleEnder.find(iCurrentSample);
	if (EndSampleIterator != m_ScheduleEnder.end())
	{
		std::unordered_set<CInstrument*> setToEnd = EndSampleIterator->second;
		for (CInstrument* instToEnd : setToEnd)
			instToEnd->noteOff();
	}
	iCurrentSample++;
}

Error_t CScheduler::add(CInstrument* rInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{

	int iOnsetInSamp = convertSecToSamp(fOnsetInSec);
	int iDurationInSamp = convertSecToSamp(fOnsetInSec);
	m_ScheduleStarter[iCurrentSample + iOnsetInSamp].insert(rInstrumentToAdd);
	m_ScheduleEnder[iCurrentSample + iDurationInSamp].insert(rInstrumentToAdd);
	return Error_t::kNoError;
}

int CScheduler::convertSecToSamp(float fSec) const
{
	return static_cast<int>(fSec * m_fSampleRateInHz);
}

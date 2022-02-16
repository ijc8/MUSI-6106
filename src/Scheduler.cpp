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
	float fCurrentValue = 0.0f;
	for (CInstrument* instToProcess : m_InstrumentVector)
		fCurrentValue += instToProcess->process();
	return fCurrentValue;
}

Error_t CScheduler::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
{
	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
		return Error_t::kFunctionInvalidArgsError;

	int iOnsetInSamp = convertSecToSamp(fOnsetInSec);
	int iDurationInSamp = convertSecToSamp(fDurationInSec);
	m_ScheduleStarter[iCurrentSample + iOnsetInSamp].insert(pInstrumentToAdd);
	m_ScheduleEnder[iCurrentSample + iOnsetInSamp + iDurationInSamp].insert(pInstrumentToAdd);
	m_InstrumentVector.push_back(pInstrumentToAdd);
	return Error_t::kNoError;
}

Error_t CScheduler::enableLoop(bool bShouldEnable)
{
	return Error_t();
}

int CScheduler::convertSecToSamp(float fSec) const
{
	return static_cast<int>(fSec * m_fSampleRateInHz);
}

#include "Scheduler.h"

int secToSamp(float sec, float sampleRate)
{
	return static_cast<int>(sec * sampleRate);
}

Scheduler::~Scheduler()
{
	for (CInstrument* inst : setInsts)
		delete inst;
}

void Scheduler::pushInst(CInstrument* instrumentToPush, float duration, float onset)
{
	int releaseInSamp = secToSamp(instrumentToPush->getADSRParameters().release, m_fSampleRateInHz);
	int durationInSamp = secToSamp(duration, m_fSampleRateInHz);
	int noteOn = secToSamp(onset, m_fSampleRateInHz);
	int totalSampleLength = noteOn + durationInSamp;
	int noteOff = totalSampleLength - releaseInSamp;
	assert(noteOff > noteOn);

	mapNoteOn[noteOn].insert(instrumentToPush);
	mapNoteOff[noteOff].insert(instrumentToPush);
	setInsts.insert(instrumentToPush);

	if (totalSampleLength > scheduleLength)
		scheduleLength = totalSampleLength;
}

float Scheduler::process()
{
	unordered_set noteOnSet = checkTriggers(sampleCounter, mapNoteOn);
	for (CInstrument* noteOnInst : noteOnSet)
		noteOnInst->noteOn();

	unordered_set noteOffSet = checkTriggers(sampleCounter, mapNoteOff);
	for (CInstrument* noteOffInst : noteOffSet)
		noteOffInst->noteOff();
	
	float currentValue = 0;
	for (CInstrument* inst : setInsts)
		currentValue += inst->process();

	sampleCounter++;
	return currentValue;
}

unordered_set<CInstrument*> Scheduler::checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck)
{
	auto triggerSample = mapToCheck.find(currentSample);
	if (triggerSample != mapToCheck.end())
	{
		unordered_set setToReturn = triggerSample->second;
		return setToReturn;
	}
	return unordered_set<CInstrument*>();
}

int Scheduler::getLength() const
{
	return scheduleLength;
}

float Looper::process()
{
	float currentValue = Scheduler::process();
	sampleCounter %= scheduleLength;
	return currentValue;
}

void Looper::setLoopLength(float newLoopLength)
{
	assert(newLoopLength > 0);
	scheduleLength = secToSamp(newLoopLength, m_fSampleRateInHz);
}













































//#include "Scheduler.h"
//
////==========================================================
//CLooper::CLooper(float fSampleRate) :
//	CSoundProcessor(fSampleRate)
//{
//}
//
//CLooper::~CLooper()
//{
//	reset();
//}
//
//float CLooper::process()
//{
//	auto startSampleIterator = m_ScheduleNoteOn.find(iCurrentSample);
//	if (startSampleIterator != m_ScheduleNoteOn.end())
//	{
//		std::unordered_set<CInstrument*> setToStart = startSampleIterator->second;
//		for (CInstrument* instToStart : setToStart)
//		{
//			instToStart->noteOn();
//		}
//	}
//	auto EndSampleIterator = m_ScheduleNoteOff.find(iCurrentSample);
//	if (EndSampleIterator != m_ScheduleNoteOff.end())
//	{
//		std::unordered_set<CInstrument*> setToEnd = EndSampleIterator->second;
//		for (CInstrument* instToEnd : setToEnd)
//		{
//			instToEnd->noteOff();
//		}
//	}
//
//	iCurrentSample++;
//	iCurrentSample %= iLoopSample;
//
//	float fCurrentValue = 0.0f;
//	for (CInstrument* instToProcess : m_InstrumentList)
//		fCurrentValue += instToProcess->process();
//
//	return fCurrentValue;
//}
//
//Error_t CLooper::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	if (addToADSRSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec) == Error_t::kNoError)
//	{
//		int iDoneSamp = convertSecToSamp(fOnsetInSec + fDurationInSec);
//		if (iDoneSamp > iLoopSample)
//			iLoopSample = iDoneSamp;
//		return Error_t::kNoError;
//	};
//	return Error_t::kFunctionInvalidArgsError;
//}
//
//Error_t CLooper::add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	CInstrument* pInstrumentToAdd = &rInstrumentToAdd;
//	return add(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
//}
//
//int CLooper::convertSecToSamp(float fSec) const
//{
//	return static_cast<int>(fSec * m_fSampleRateInHz);
//}
//int CLooper::convertSampToSec(int iSamp) const
//{
//	return static_cast<float>(iSamp / m_fSampleRateInHz);
//}
//Error_t CLooper::addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	if (pInstrumentToAdd == nullptr || fOnsetInSec < 0 || fDurationInSec < 0)
//		return Error_t::kFunctionInvalidArgsError;
//
//	int iNoteOnInSamp = convertSecToSamp(fOnsetInSec) + iCurrentSample;
//	int iNoteOffInSamp = iNoteOnInSamp + convertSecToSamp(fDurationInSec - pInstrumentToAdd->getADSRParameters().release);
//	assert(iNoteOffInSamp > iNoteOnInSamp);
//	if (iNoteOffInSamp < iNoteOnInSamp)
//		return Error_t::kFunctionInvalidArgsError;
//
//	m_ScheduleNoteOn[iNoteOnInSamp].insert(pInstrumentToAdd);
//	m_ScheduleNoteOff[iNoteOffInSamp].insert(pInstrumentToAdd);
//	m_InstrumentList.push_front(pInstrumentToAdd);
//
//	return Error_t::kNoError;
//}
//
//Error_t CLooper::reset()
//{
//	for (CInstrument* instToDelete : m_GarbageCollector)
//		delete instToDelete;
//	m_ScheduleNoteOn.clear();
//	m_ScheduleNoteOff.clear();
//	m_InstrumentList.clear();
//	m_GarbageCollector.clear();
//	iCurrentSample = 0;
//	iLoopSample = 1;
//	assert(m_GarbageCollector.empty());
//	return Error_t::kNoError;
//}
//
////==========================================================================
//
//CScheduler::CScheduler(float fSampleRate) :
//	CLooper(fSampleRate)
//{
//}
//
//CScheduler::~CScheduler()
//{
//	reset();
//}
//
//float CScheduler::process()
//{
//	auto noteOnIterator = m_ScheduleNoteOn.find(iCurrentSample);
//	if (noteOnIterator != m_ScheduleNoteOn.end())
//	{
//		std::unordered_set<CInstrument*> setToStart = noteOnIterator->second;
//		for (CInstrument* instToStart : setToStart)
//		{
//			instToStart->noteOn();
//		}
//		m_ScheduleNoteOn.erase(noteOnIterator);
//	}
//	auto noteOffIterator = m_ScheduleNoteOff.find(iCurrentSample);
//	if (noteOffIterator != m_ScheduleNoteOff.end())
//	{
//		std::unordered_set<CInstrument*> setToEnd = noteOffIterator->second;
//		for (CInstrument* instToEnd : setToEnd)
//		{
//			instToEnd->noteOff();
//		}
//		m_ScheduleNoteOff.erase(noteOffIterator);
//	}
//	auto removeIterator = m_ScheduleRemover.find(iCurrentSample);
//	if (removeIterator != m_ScheduleRemover.end())
//	{
//		std::unordered_set<CInstrument*> setToRemove = removeIterator->second;
//		for (CInstrument* instToRemove : setToRemove)
//		{
//			m_InstrumentList.remove(instToRemove);
//		}
//		m_ScheduleRemover.erase(removeIterator);
//	}
//	iCurrentSample++;
//
//	float fCurrentValue = 0.0f;
//	for (CInstrument* instToProcess : m_InstrumentList)
//		fCurrentValue += instToProcess->process();
//
//	return fCurrentValue;
//}
//
//Error_t CScheduler::add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	m_GarbageCollector.push_back(pInstrumentToAdd);
//	return addToSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
//}
//
//Error_t CScheduler::add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	CInstrument* pInstrumentToAdd = &rInstrumentToAdd;
//	return addToSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
//}
//
//Error_t CScheduler::reset()
//{
//	m_ScheduleRemover.clear();
//	return CLooper::reset();
//}
//
//Error_t CScheduler::addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	return CLooper::addToADSRSchedulers(pInstrumentToAdd, fOnsetInSec + convertSampToSec(iCurrentSample), fDurationInSec);
//}
//
//Error_t CScheduler::addToInstRemover(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	int iDoneSamp = convertSecToSamp(fOnsetInSec + fDurationInSec) + iCurrentSample;
//	m_ScheduleRemover[iDoneSamp].insert(pInstrumentToAdd);
//	return Error_t::kNoError;
//}
//Error_t CScheduler::addToSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec)
//{
//	if (m_InstrumentList.empty())
//		iCurrentSample = 0;
//	if (addToADSRSchedulers(pInstrumentToAdd, fOnsetInSec, fDurationInSec) == Error_t::kNoError)
//		return addToInstRemover(pInstrumentToAdd, fOnsetInSec, fDurationInSec);
//	return Error_t::kFunctionInvalidArgsError;
//}
////==========================================================
//
//


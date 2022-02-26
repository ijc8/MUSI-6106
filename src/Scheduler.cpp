#include "Scheduler.h"

CScheduler::~CScheduler()
{
	for (CSoundProcessor* inst : m_GarbageCollector)
		delete inst;
}

void CScheduler::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	int iReleaseInSamp = secToSamp(pInstToPush->getADSRParameters().release, m_fSampleRateInHz);
	int iDurationInSamp = secToSamp(fDurationInSec, m_fSampleRateInHz);
	int iNoteOn = secToSamp(fOnsetInSec, m_fSampleRateInHz);
	int iTotalLengthInSamp = iNoteOn + iDurationInSamp;
	int iNoteOff = iTotalLengthInSamp - iReleaseInSamp;
	assert(iNoteOff > iNoteOn);

	m_MapNoteOn[iNoteOn].insert(pInstToPush);
	m_MapNoteOff[iNoteOff].insert(pInstToPush);
	m_MapRemover[iTotalLengthInSamp].insert(pInstToPush);

	m_GarbageCollector.insert(pInstToPush);

	if (iTotalLengthInSamp > m_iScheduleLength)
		m_iScheduleLength = iTotalLengthInSamp;
}

void CScheduler::noteOn()
{
	m_iSampleCounter = 0;
	m_adsr.noteOn();
}

void CScheduler::noteOff()
{
	m_adsr.noteOff();
}

void CScheduler::process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
	if (m_adsr.isActive())
	{
		auto itNoteOn = m_MapNoteOn.find(m_iSampleCounter);
		if (itNoteOn != m_MapNoteOn.end())
		{
			for (CInstrument* inst : itNoteOn->second)
			{
				inst->noteOn();
				m_SetInsts.insert(inst);
			}
		}

		auto itNoteOff = m_MapNoteOff.find(m_iSampleCounter);
		if (itNoteOff != m_MapNoteOff.end())
		{
			for (CInstrument* inst : itNoteOff->second)
			{
				inst->noteOff();
			}
		}

		auto itRemover = m_MapRemover.find(m_iSampleCounter);
		if (itRemover != m_MapRemover.end())
		{
			for (CInstrument* inst : itRemover->second)
			{
				m_SetInsts.erase(inst);
			}
		}

		for (CInstrument* inst : m_SetInsts)
			inst->process(ppfOutBuffer, iNumChannels, iCurrentFrame);

		m_iSampleCounter++;
	}
}

int CScheduler::getLength() const
{
	return m_iScheduleLength;
}

void CLooper::process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
	CScheduler::process(ppfOutBuffer, iNumChannels, iCurrentFrame);
	m_iSampleCounter %= m_iScheduleLength;
}

void CLooper::setLoopLength(float fNewLoopLengthInSec)
{
	assert(fNewLoopLengthInSec > 0);
	m_iScheduleLength = secToSamp(fNewLoopLengthInSec, m_fSampleRateInHz);
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


#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	m_SetInsts.insert(&rInstToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	m_SetInsts.erase(&rInstToRemove);
}

void CMainProcessor::addScheduleRef(CScheduler& rScheduleToAdd)
{
	m_SetSchedules.insert(&rScheduleToAdd);
}

void CMainProcessor::removeScheduleRef(CScheduler& scheduleToRemove)
{
	m_SetSchedules.erase(&scheduleToRemove);
}

void CMainProcessor::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	CScheduler::pushInst(pInstToPush, fOnsetInSec + sampToSec(m_iSampleCounter, m_fSampleRateInHz), fDurationInSec);
	int iDeleteSample = secToSamp(fOnsetInSec + fDurationInSec, m_fSampleRateInHz) + m_iSampleCounter;
	m_InstRemover[iDeleteSample].insert(pInstToPush);
}

void CMainProcessor::pushSchedule(CScheduler* pScheduleToPush)
{
	m_SetSchedules.insert(pScheduleToPush);
	m_GarbageCollector.insert(pScheduleToPush);
	int iDeleteSample = pScheduleToPush->getLength() + m_iSampleCounter;
	m_ScheduleRemover[iDeleteSample].insert(pScheduleToPush);
	pScheduleToPush->start();
}

void CMainProcessor::pushLooper(CLooper* pLoopToPush, int iNumTimesToLoop)
{
	m_SetSchedules.insert(pLoopToPush);
	m_GarbageCollector.insert(pLoopToPush);
	int iDeleteSample = pLoopToPush->getLength() * iNumTimesToLoop + m_iSampleCounter;
	m_ScheduleRemover[iDeleteSample].insert(pLoopToPush);
	pLoopToPush->start();
}

void CMainProcessor::process(float** ppfOutBuffer, int iNumChannels, int iNumSamples, const int& iMasterClock)
{
	// removes any temporary instruments that are done processing
	auto itUpperBoundInst = m_InstRemover.upper_bound(m_iSampleCounter);
	for (auto it = m_InstRemover.begin(); it != itUpperBoundInst; it++)
	{
		for (CInstrument* pInst : it->second)
		{
			m_SetInsts.erase(pInst);
		}
	}
	m_InstRemover.erase(m_InstRemover.begin(), itUpperBoundInst);

	// removes any temporary schedules/loops that are done processing
	auto itUpperBoundSchedule = m_ScheduleRemover.upper_bound(m_iSampleCounter);
	for (auto it = m_ScheduleRemover.begin(); it != itUpperBoundSchedule; it++)
	{
		for (CScheduler* pSchedule : it->second)
		{
			m_SetSchedules.erase(pSchedule);
		}
	}
	m_ScheduleRemover.erase(m_ScheduleRemover.begin(), itUpperBoundSchedule);

	for (CScheduler* pSchedule : m_SetSchedules)
		pSchedule->process(ppfOutBuffer, iNumChannels, iNumSamples, m_iSampleCounter);
	CScheduler::process(ppfOutBuffer, iNumChannels, iNumSamples, m_iSampleCounter);
}
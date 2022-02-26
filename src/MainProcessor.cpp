#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	m_SetInsts.insert(&rInstToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	m_SetInsts.erase(&rInstToRemove);
}

void CMainProcessor::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	CScheduler::pushInst(pInstToPush, fOnsetInSec + sampToSec(m_iSampleCounter, m_fSampleRateInHz), fDurationInSec);
}


void CMainProcessor::process(float** ppfOutBuffer, int iNumChannels, int iNumFrames)
{
	for (int frame = 0; frame < iNumFrames; frame++)
	{
		CScheduler::process(ppfOutBuffer, iNumChannels, frame);
	}
}

unordered_set<CInstrument*> CMainProcessor::checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck)
{
	auto triggerSample = mapToCheck.find(currentSample);
	if (triggerSample != mapToCheck.end())
	{
		unordered_set setToReturn = triggerSample->second;
		mapToCheck.erase(triggerSample);
		return setToReturn;
	}
	return unordered_set<CInstrument*>();
}


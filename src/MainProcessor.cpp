#include "MainProcessor.h"

long long CMainProcessor::s_iMasterClock = 0;

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
	CScheduler::pushInst(pInstToPush, fOnsetInSec + sampToSec(s_iMasterClock, m_fSampleRateInHz), fDurationInSec);
	int iDeleteSample = secToSamp(fOnsetInSec + fDurationInSec, m_fSampleRateInHz) + s_iMasterClock;
	m_InstRemover[iDeleteSample].insert(pInstToPush);
}


void CMainProcessor::process(float** ppfOutBuffer, int iNumChannels, int iNumFrames)
{
	for (int frame = 0; frame < iNumFrames; frame++)
	{
		CScheduler::process(ppfOutBuffer, iNumChannels, frame);
		s_iMasterClock++;
	}
}


#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	m_SetInsts.insert(&rInstToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	m_SetInsts.erase(&rInstToRemove);
}

Error_t CMainProcessor::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	return CScheduler::pushInst(pInstToPush, fOnsetInSec + sampToSec(m_iSampleCounter, m_fSampleRateInHz), fDurationInSec);
}


void CMainProcessor::process(float** ppfOutBuffer, int iNumChannels, int iNumFrames)
{
	for (int frame = 0; frame < iNumFrames; frame++)
	{	
		CScheduler::processFrame(ppfOutBuffer, iNumChannels, frame);
	}

}


void CMainProcessor::checkTriggers()
{
	CScheduler::checkTriggers();
	m_MapNoteOn.erase(m_iSampleCounter);
	m_MapNoteOff.erase(m_iSampleCounter);
	m_MapRemover.erase(m_iSampleCounter);
}


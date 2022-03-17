#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	m_SetInsts.insert(&rInstToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	m_InstRefRemover.push_back(&rInstToRemove);
}

bool CMainProcessor::contains(CInstrument& rInstToCheck)
{
	auto it = m_SetInsts.find(&rInstToCheck);
	return (it != m_SetInsts.end());
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
	for (CInstrument* inst : m_InstRefRemover)
	{
		m_SetInsts.erase(inst);
	}
	m_InstRefRemover.clear();
}


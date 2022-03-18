#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	const juce::ScopedLock lock(m_Lock);
	m_SetInsts.insert(&rInstToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	const juce::ScopedLock lock(m_Lock);
	m_SetInsts.erase(&rInstToRemove);
}

Error_t CMainProcessor::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	const juce::ScopedLock lock(m_Lock);
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

	const juce::ScopedLock lock(m_Lock);
	m_MapNoteOn.erase(m_iSampleCounter);
	m_MapNoteOff.erase(m_iSampleCounter);
	m_MapRemover.erase(m_iSampleCounter);
}


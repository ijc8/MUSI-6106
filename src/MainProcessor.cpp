#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	auto instToAdd = std::make_pair(&rInstToAdd, std::nullopt);
	m_InsertQueue.push(instToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	const juce::ScopedLock lock(m_Lock);
	m_SetInsts.erase(&rInstToRemove);
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

void CMainProcessor::checkInsertQueue()
{
	// Places event and instrument pointer into appropriate container
	std::pair<CInstrument*, std::optional<TriggerInfo>> instToAdd;
	while (m_InsertQueue.pop(instToAdd))
	{
		CInstrument* pInstToAdd = instToAdd.first;
		auto triggerInfo = instToAdd.second;
		if (triggerInfo.has_value())
		{
			m_MapNoteOn[triggerInfo.value().noteOn + m_iSampleCounter].insert(pInstToAdd);
			m_MapNoteOff[triggerInfo.value().noteOff + m_iSampleCounter].insert(pInstToAdd);
			m_MapRemover[triggerInfo.value().remove + m_iSampleCounter].insert(pInstToAdd);
		}
		else
		{
			m_SetInsts.insert(pInstToAdd);
		}
	}
}


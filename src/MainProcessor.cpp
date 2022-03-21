#include "MainProcessor.h"

Error_t CMainProcessor::addInstRef(CInstrument& rInstToAdd)
{
	auto instToAdd = std::make_pair(&rInstToAdd, std::nullopt);
	if (!m_InsertQueue.push(instToAdd))
		return Error_t::kUnknownError;
	return Error_t::kNoError;
}

Error_t CMainProcessor::removeInstRef(CInstrument& rInstToRemove)
{
	if (!m_RemoveQueue.push(&rInstToRemove))
		return Error_t::kUnknownError;
	return Error_t::kNoError;
}

Error_t CMainProcessor::removeInst(CInstrument*& pInstToRemove)
{
	if (!pInstToRemove)
		return Error_t::kMemError;

	if (!m_RemoveQueue.push(pInstToRemove))
		return Error_t::kUnknownError;
	return Error_t::kNoError;
}

Error_t CMainProcessor::addInst(CInstrument*& pInstToAdd)
{
	if (!pInstToAdd)
		return Error_t::kMemError;

	if (!m_InsertQueue.push(std::pair(pInstToAdd, std::nullopt)))
		return Error_t::kUnknownError;

	m_GarbageCollector.insert(pInstToAdd);
	return Error_t::kNoError;
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

void CMainProcessor::checkQueues()
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

	CInstrument* instToRemove = 0;
	while (m_RemoveQueue.pop(instToRemove))
	{
		m_SetInsts.erase(instToRemove);
	}
}



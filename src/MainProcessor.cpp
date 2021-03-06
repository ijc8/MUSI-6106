#include "MainProcessor.h"

Error_t CMainProcessor::addInst(std::shared_ptr<CInstrument> pInstToAdd)
{
	if (!pInstToAdd)
		return Error_t::kMemError;

	// Creates an instrument paired with NO trigger information
	std::pair<std::shared_ptr<CInstrument>, std::optional<TriggerInfo>> instToAdd = std::pair(pInstToAdd, std::nullopt);

	if (!m_InsertQueue.push(instToAdd))
		return Error_t::kUnknownError;

	return Error_t::kNoError;
}

Error_t CMainProcessor::removeInst(std::shared_ptr<CInstrument> pInstToRemove)
{
	if (!pInstToRemove)
		return Error_t::kMemError;

	if (!m_RemoveQueue.push(pInstToRemove))
		return Error_t::kUnknownError;

	return Error_t::kNoError;
}

void CMainProcessor::process(float** ppfOutBuffer, int iNumChannels, int iNumFrames)
{
	for (int frame = 0; frame < iNumFrames; frame++)
	{	
		CScheduler::processFrame(ppfOutBuffer, iNumChannels, frame);
	}

}


void CMainProcessor::checkFlags()
{
	CInstrument::checkFlags();
}

void CMainProcessor::checkTriggers()
{
	CScheduler::checkTriggers();

	// Erases data entries that will never again be accessed
	m_MapNoteOn.erase(m_iSampleCounter.load());
	m_MapNoteOff.erase(m_iSampleCounter.load());
	m_MapRemover.erase(m_iSampleCounter.load());
}

void CMainProcessor::checkQueues()
{
	// Places event and instrument pointer into appropriate container
	std::pair<std::shared_ptr<CInstrument>, std::optional<TriggerInfo>> instToAdd;
    int insertMax = 32;
	while (m_InsertQueue.pop(instToAdd))
	{
		std::shared_ptr<CInstrument> pInstToAdd = instToAdd.first;
		auto triggerInfo = instToAdd.second;
		if (triggerInfo.has_value())
		{
			// Here m_iSampleCounter is added to trigger values so that they are relative to the CURRENT POSITION IN TIME
			m_MapNoteOn[static_cast<int64_t>(triggerInfo.value().noteOn) + m_iSampleCounter.load()].insert(pInstToAdd);
			m_MapNoteOff[static_cast<int64_t>(triggerInfo.value().noteOff) + m_iSampleCounter.load()].insert(pInstToAdd);
			m_MapRemover[static_cast<int64_t>(triggerInfo.value().remove) + m_iSampleCounter.load()].insert(pInstToAdd);
		}
		else
		{
			m_ActiveInsts.insert(pInstToAdd);
		}
		m_AllInsts.insert(pInstToAdd);
	}

	std::shared_ptr<CInstrument> instToRemove = 0;
    int removeMax = 32;
	while (m_RemoveQueue.pop(instToRemove))
	{
		m_ActiveInsts.erase(instToRemove);
		m_AllInsts.erase(instToRemove);
		m_GarbageQueue.push(instToRemove);
	}
}

void CMainProcessor::timerCallback()
{
	std::shared_ptr<CInstrument> instToTrash = 0;
	while (m_GarbageQueue.pop(instToTrash))
	{
		if (instToTrash.unique())
			instToTrash.reset();
	}
}



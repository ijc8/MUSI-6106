#include "Scheduler.h"

CScheduler::CScheduler(float sampleRate) :
	CInstrument(1.0f, sampleRate)
{

	// Allocate memory to be used as a temporary buffer in the processing function
	// I arbitrarily am using a max channels value of 6 to be safe, but this can likely be passed in as a parameter somewhere
	m_ppfTempBuffer = new float*[m_iMaxChannels];
	for (int channel = 0; channel < m_iMaxChannels; channel++)
		m_ppfTempBuffer[channel] = new float[1] {0};

}

CScheduler::~CScheduler()
{
	for (int channel = 0; channel < m_iMaxChannels; channel++)
		delete[] m_ppfTempBuffer[channel];
	delete[] m_ppfTempBuffer;
}

Error_t CScheduler::scheduleInst(std::unique_ptr<CInstrument> pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	if (pInstToPush == nullptr || fOnsetInSec < 0 || fDurationInSec <= 0)
		return Error_t::kFunctionInvalidArgsError;

	/// Computes location and event information
	int iReleaseInSamp = secToSamp(pInstToPush->getADSRParameters().release, m_fSampleRateInHz);
	int iDurationInSamp = secToSamp(fDurationInSec, m_fSampleRateInHz);
	int iNoteOn = secToSamp(fOnsetInSec, m_fSampleRateInHz);
	int iTotalLengthInSamp = iNoteOn + iDurationInSamp;
	int iNoteOff = iTotalLengthInSamp - iReleaseInSamp;
	if (iNoteOff < iNoteOn)
		return Error_t::kFunctionInvalidArgsError;

	TriggerInfo triggerInfo = TriggerInfo(iNoteOn, iNoteOff, iTotalLengthInSamp);
	auto instToPush = std::make_pair(std::shared_ptr(std::move(pInstToPush)), std::make_optional(triggerInfo));

	m_InsertQueue.push(instToPush);
	
	// Adjusts length of the entire container
	if (iTotalLengthInSamp > m_iScheduleLength)
	{
		m_iScheduleLength.store(iTotalLengthInSamp);
	}


	return Error_t::kNoError;
}

void CScheduler::processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
	// Checks for state changes i.e. noteOn() and/or noteOff() calls
	checkFlags();

	// Checks insert and remove queues
	checkQueues();
	
	// Parses each map and sees if any event triggers exist for current sample counter		
	// Carries out necessary actions if so
	checkTriggers();

	// Place child instrument values into a temporary, single-frame buffer
	// If you get a read access error here, one of the objects in m_SetInsts probably went out of scope and deallocated itself
	for (std::shared_ptr<CInstrument> inst : m_SetInsts)				
		inst->processFrame(m_ppfTempBuffer, iNumChannels, 0);

	m_iSampleCounter++;


	// Apply the schedule adsr and gain to this temporary buffer, THEN place into main output buffer
	float fAdsrValue = m_adsr.getNextSample();
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		float fPanGain{ 0 };
		if (channel == 0) 
		{
			fPanGain = (1.0f - m_fPan);
		}
		if (channel == 1)
		{
			fPanGain = m_fPan;
		}
		ppfOutBuffer[channel][iCurrentFrame] += m_fGain * fAdsrValue * (iNumChannels * fPanGain) * m_ppfTempBuffer[channel][0];
		m_ppfTempBuffer[channel][0] = 0;
	}

		
}

void CScheduler::checkFlags()
{

	if (m_bNoteOnPressed.load())
	{
		for (std::shared_ptr<CInstrument> inst : m_SetInsts)
			inst->resetADSR();
		m_iSampleCounter.store(0);
		m_bNoteOnPressed.store(false);
		m_adsr.noteOn();
	}

	if (m_bNoteOffPressed.load())
	{
		m_bNoteOffPressed.store(false);
		m_adsr.noteOff();
	}
}

void CScheduler::checkTriggers()
{
	auto noteOnTrigger = m_MapNoteOn.find(m_iSampleCounter);
	if (noteOnTrigger != m_MapNoteOn.end())
	{
		for (std::shared_ptr<CInstrument> inst : noteOnTrigger->second)
		{
			inst->noteOn();
			m_SetInsts.insert(inst);
		}
	}
	
	auto noteOffTrigger = m_MapNoteOff.find(m_iSampleCounter);
	if (noteOffTrigger != m_MapNoteOff.end())
	{
		for (std::shared_ptr<CInstrument> inst : noteOffTrigger->second)
		{
			inst->noteOff();
		}
	}

	auto removeTrigger = m_MapRemover.find(m_iSampleCounter);
	if (removeTrigger != m_MapRemover.end())
	{
		for (std::shared_ptr<CInstrument> inst : removeTrigger->second)
		{
			m_SetInsts.erase(inst);
		}
	}
}

void CScheduler::checkQueues()
{

	// Places event and instrument pointer into appropriate container
	std::pair<std::shared_ptr<CInstrument>, std::optional<TriggerInfo>> instToAdd;
	while (m_InsertQueue.pop(instToAdd))
	{
		std::shared_ptr<CInstrument> pInstToAdd = instToAdd.first;
		auto triggerInfo = instToAdd.second;
		m_MapNoteOn[static_cast<int64_t>(triggerInfo.value().noteOn)].insert(pInstToAdd);
		m_MapNoteOff[static_cast<int64_t>(triggerInfo.value().noteOff)].insert(pInstToAdd);
		m_MapRemover[static_cast<int64_t>(triggerInfo.value().remove)].insert(pInstToAdd);
	}
}

int CScheduler::getLengthInSamp() const
{
	return m_iScheduleLength.load();
}

float CScheduler::getLengthInSec() const
{
	return sampToSec(m_iScheduleLength.load(), m_fSampleRateInHz);
}

void CLooper::processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
	CScheduler::processFrame(ppfOutBuffer, iNumChannels, iCurrentFrame);

	// Wraps around internal clock to allow for looping
	m_iSampleCounter.store(m_iSampleCounter.load() % m_iScheduleLength.load());
}

Error_t CLooper::setLoopLength(float fNewLoopLengthInSec)
{
	if (fNewLoopLengthInSec <= 0)
		return Error_t::kFunctionInvalidArgsError;

	m_iScheduleLength.store(secToSamp(fNewLoopLengthInSec, m_fSampleRateInHz));
	return Error_t::kNoError;
}

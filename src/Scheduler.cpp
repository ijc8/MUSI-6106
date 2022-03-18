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

	for (CInstrument* inst : m_GarbageCollector)
	{
		delete inst;
	}
}

Error_t CScheduler::pushInst(CInstrument* pInstToPush, float fOnsetInSec, float fDurationInSec)
{
	if (pInstToPush == nullptr || fOnsetInSec < 0 || fDurationInSec <= 0)
		return Error_t::kFunctionInvalidArgsError;

	/// Computes location and event information
	int64_t iReleaseInSamp = secToSamp(pInstToPush->getADSRParameters().release, m_fSampleRateInHz);
	int64_t iDurationInSamp = secToSamp(fDurationInSec, m_fSampleRateInHz);
	int64_t iNoteOn = secToSamp(fOnsetInSec, m_fSampleRateInHz);
	int64_t iTotalLengthInSamp = iNoteOn + iDurationInSamp;
	int64_t iNoteOff = iTotalLengthInSamp - iReleaseInSamp;
	if (iNoteOff < iNoteOn)
		return Error_t::kFunctionInvalidArgsError;

	{
		// Places event and instrument pointer into appropriate container
		const juce::ScopedLock lock(m_Lock);
		m_MapNoteOn[iNoteOn].insert(pInstToPush);
		m_MapNoteOff[iNoteOff].insert(pInstToPush);
		m_MapRemover[iTotalLengthInSamp].insert(pInstToPush);
		m_GarbageCollector.insert(pInstToPush);
	}
	
	// Adjusts length of the entire container
	if (iTotalLengthInSamp > m_iScheduleLength)
	{
		m_iScheduleLength = iTotalLengthInSamp;
	}


	return Error_t::kNoError;
}

void CScheduler::noteOn()
{
	m_iSampleCounter = 0;
	for (CInstrument* inst : m_SetInsts)
		inst->resetADSR();
	CInstrument::noteOn();
}

void CScheduler::processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
		// Parses each map and sees if any event triggers exist for current sample counter
		// Carries out necessary actions if so
		checkTriggers();

		// Place child instrument values into a temporary, single-frame buffer
		// If you get a read access error here, one of the objects in m_SetInsts probably went out of scope and deallocated itself
		{
			const juce::ScopedLock lock(m_Lock);
			for (CInstrument* inst : m_SetInsts)
				inst->processFrame(m_ppfTempBuffer, iNumChannels, 0);
		}


		// Apply the schedule adsr and gain to this temporary buffer, THEN place into main output buffer
		float fAdsrValue = m_adsr.getNextSample();
		for (int channel = 0; channel < iNumChannels; channel++)
		{
			float fPanGain{ 0 };
			if (channel == 0) {
				fPanGain = (1.0f - m_fPan);
			}
			if (channel == 1) {
				fPanGain = m_fPan;
			}
			ppfOutBuffer[channel][iCurrentFrame] += m_fGain * fAdsrValue * (iNumChannels * fPanGain) * m_ppfTempBuffer[channel][0];
			m_ppfTempBuffer[channel][0] = 0;
		}

		m_iSampleCounter++;


}

void CScheduler::checkTriggers()
{
	auto noteOnTrigger = m_MapNoteOn.find(m_iSampleCounter);
	if (noteOnTrigger != m_MapNoteOn.end())
	{
		const juce::ScopedLock lock(m_Lock);
		for (CInstrument* inst : noteOnTrigger->second)
		{
			inst->noteOn();
			m_SetInsts.insert(inst);
		}
	}
	
	auto noteOffTrigger = m_MapNoteOff.find(m_iSampleCounter);
	if (noteOffTrigger != m_MapNoteOff.end())
	{
		for (CInstrument* inst : noteOffTrigger->second)
		{
			inst->noteOff();
		}
	}

	auto removeTrigger = m_MapRemover.find(m_iSampleCounter);
	if (removeTrigger != m_MapRemover.end())
	{
		const juce::ScopedLock lock(m_Lock);
		for (CInstrument* inst : removeTrigger->second)
		{
			m_SetInsts.erase(inst);
		}
	}
}

int64_t CScheduler::getLengthInSamp() const
{
	return m_iScheduleLength;
}

float CScheduler::getLengthInSec() const
{
	return sampToSec(m_iScheduleLength, m_fSampleRateInHz);
}

void CLooper::processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame)
{
	CScheduler::processFrame(ppfOutBuffer, iNumChannels, iCurrentFrame);

	// Wraps around internal clock to allow for looping
	m_iSampleCounter %= m_iScheduleLength;
}

Error_t CLooper::setLoopLength(float fNewLoopLengthInSec)
{
	if (fNewLoopLengthInSec <= 0)
		return Error_t::kFunctionInvalidArgsError;

	m_iScheduleLength = secToSamp(fNewLoopLengthInSec, m_fSampleRateInHz);
	return Error_t::kNoError;
}

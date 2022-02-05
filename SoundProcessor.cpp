#include "SoundProcessor.h"

//=======================================================================
CSoundProcessor::CSoundProcessor() :
	m_fGain(0.0)
{

}

CSoundProcessor::~CSoundProcessor()
{

}

Error_t CSoundProcessor::setSampleRate(float fNewSampleRate)
{
	assert(fNewSampleRate >= 0.0);
	if (fNewSampleRate < 0.0)
		return Error_t::kFunctionInvalidArgsError;

	s_fSampleRateInHz = fNewSampleRate;
	return Error_t::kNoError;
}


Error_t CSoundProcessor::setGain(float fNewGain)
{
	assert(fNewGain > -1.0 && fNewGain < 1.0);
	if (fNewGain < -1.0 || fNewGain > 1.0)
		return Error_t::kFunctionInvalidArgsError;

	m_fGain = fNewGain;
	return Error_t::kNoError;
}

float CSoundProcessor::getGain() const
{
	return m_fGain;
}

float CSoundProcessor::s_fSampleRateInHz = 0.0f;
//=======================================================================

//=======================================================================
COscillator::COscillator(const Wavetable& wavetableToUse, float fFrequency) :
	m_fCurrentIndex(0.0f),
	m_fTableDelta(0.0f),
	m_Wavetable(wavetableToUse),
	FREQ_TO_TABLEDELTA( (float)wavetableToUse.getNumSamples() / s_fSampleRateInHz),
	TABLEDELTA_TO_FREQ( 1.0f / FREQ_TO_TABLEDELTA)
{
	setFrequency(fFrequency);
}

COscillator::~COscillator()
{

}

Error_t COscillator::create(CSoundProcessor*& pCSoundProcessor, const Wavetable& wavetableToUse, float fFrequency)
{
	assert(!pCSoundProcessor);
	if (!pCSoundProcessor) 
	{
		pCSoundProcessor = new COscillator(wavetableToUse, fFrequency);
		return Error_t::kNoError;
	}
	return Error_t::kMemError;
}

Error_t COscillator::destroy(CSoundProcessor*& pCSoundProcessor)
{
	delete pCSoundProcessor;
	pCSoundProcessor = 0;
	return Error_t::kNoError;
}

Error_t COscillator::setFrequency(float fNewFrequency)
{
	assert(fNewFrequency > 0 && fNewFrequency < 20000);
	if (fNewFrequency < 0 || fNewFrequency > 20000)
		return Error_t::kFunctionInvalidArgsError;

	m_fTableDelta = FREQ_TO_TABLEDELTA * fNewFrequency;
	return Error_t::kNoError;
}

float COscillator::getFrequency() const
{
	return m_fTableDelta * TABLEDELTA_TO_FREQ;
}

float COscillator::process()
{
	return 0.0f;
}
//=======================================================================
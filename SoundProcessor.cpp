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
	COscillator::updateConversionFactors();
	return Error_t::kNoError;
}

float CSoundProcessor::getSampleRate()
{
	return s_fSampleRateInHz;
}


Error_t CSoundProcessor::setGain(float fNewGain)
{
	assert(fNewGain >= -1.0 && fNewGain <= 1.0);
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
COscillator::COscillator(const Wavetable& wavetableToUse, float fFrequency, float fGain) :
	m_fCurrentIndex(0.0f),
	m_fTableDelta(0.0f),
	m_Wavetable(wavetableToUse)
{
	setFrequency(fFrequency);
	setGain(fGain);
}

COscillator::~COscillator()
{

}

Error_t COscillator::create(CSoundProcessor*& pCSoundProcessor, const Wavetable& wavetableToUse, float fFrequency, float fGain)
{
	assert(!pCSoundProcessor);
	if (!pCSoundProcessor) 
	{
		pCSoundProcessor = new COscillator(wavetableToUse, fFrequency, fGain);
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

Error_t COscillator::updateConversionFactors()
{
	if (s_fSampleRateInHz == 0.0f)
	{
		s_FREQ_TO_TABLEDELTA = 0.0f;
		s_TABLEDELTA_TO_FREQ = 0.0f;
	}
	else
	{
		s_FREQ_TO_TABLEDELTA = Wavetable::getNumSamples() / s_fSampleRateInHz;
		s_TABLEDELTA_TO_FREQ = 1.0f / s_FREQ_TO_TABLEDELTA;
	}
	return Error_t::kNoError;
}

Error_t COscillator::setFrequency(float fNewFrequency)
{
	assert(fNewFrequency >= 0 && fNewFrequency <= 20000);
	if (fNewFrequency < 0 || fNewFrequency > 20000)
		return Error_t::kFunctionInvalidArgsError;

	m_fTableDelta = s_FREQ_TO_TABLEDELTA * fNewFrequency;
	return Error_t::kNoError;
}

float COscillator::getFrequency() const
{
	return m_fTableDelta * s_TABLEDELTA_TO_FREQ;
}

float COscillator::process()
{
	unsigned tableSize = (unsigned)m_Wavetable.getNumSamples();

	unsigned index0 = (unsigned)m_fCurrentIndex;
	unsigned index1 = index0 + 1;
	if (index1 >= tableSize)
		index1 = (unsigned)0;

	float frac = m_fCurrentIndex - (float)index0;

	const float* wavetableReader = m_Wavetable.getReadPointer(0);
	float value0 = wavetableReader[index0];
	float value1 = wavetableReader[index1];

	float currentSample = value0 + frac * (value1 - value0);

	if ((m_fCurrentIndex += m_fTableDelta) > (float)tableSize)
		m_fCurrentIndex -= (float)tableSize;

	return m_fGain * currentSample;
}

float COscillator::s_FREQ_TO_TABLEDELTA = 0.0f;
float COscillator::s_TABLEDELTA_TO_FREQ = 0.0f;
//=======================================================================
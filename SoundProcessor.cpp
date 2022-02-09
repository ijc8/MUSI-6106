#include "SoundProcessor.h"

//=======================================================================
CSoundProcessor::CSoundProcessor()
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
	CWavetableOscillator::updateConversionFactors();
	return Error_t::kNoError;
}

float CSoundProcessor::getSampleRate()
{
	return s_fSampleRateInHz;
}

float CSoundProcessor::s_fSampleRateInHz = 44100.0f;
//=======================================================================

//=======================================================================
CInstrument::CInstrument() :
	m_fGain(0.0f)
{

}

CInstrument::~CInstrument()
{

}

Error_t CInstrument::setGain(float fNewGain)
{
	assert(fNewGain >= -1.0 && fNewGain <= 1.0);
	if (fNewGain < -1.0 || fNewGain > 1.0)
		return Error_t::kFunctionInvalidArgsError;

	m_fGain = fNewGain;
	return Error_t::kNoError;
}

float CInstrument::getGain() const
{
	return m_fGain;
}
//=======================================================================

//=======================================================================
CWavetableOscillator::CWavetableOscillator(const Wavetable& wavetableToUse, float fFrequency, float fGain) :
	m_fFrequencyInHz(0.0f),
	m_fCurrentIndex(0.0f),
	m_fTableDelta(0.0f),
	m_Wavetable(wavetableToUse)
{
	setFrequency(fFrequency);
	setGain(fGain);
}

CWavetableOscillator::~CWavetableOscillator()
{

}

Error_t CWavetableOscillator::updateConversionFactors()
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

Error_t CWavetableOscillator::setFrequency(float fNewFrequency)
{
	assert(fNewFrequency >= 0 && fNewFrequency <= 20000);
	if (fNewFrequency < 0 || fNewFrequency > 20000)
		return Error_t::kFunctionInvalidArgsError;

	m_fFrequencyInHz = fNewFrequency;
	m_fTableDelta = s_FREQ_TO_TABLEDELTA * fNewFrequency;
	return Error_t::kNoError;
}

float CWavetableOscillator::getFrequency() const
{
	return m_fFrequencyInHz;
}

float CWavetableOscillator::process()
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

void CWavetableOscillator::reinitialize()
{
	setFrequency(m_fFrequencyInHz);
}

float CWavetableOscillator::s_FREQ_TO_TABLEDELTA = (s_fSampleRateInHz == 0) ? 0 : Wavetable::getNumSamples() / s_fSampleRateInHz;
float CWavetableOscillator::s_TABLEDELTA_TO_FREQ = (s_fSampleRateInHz == 0) ? 0 : 1.0f / s_FREQ_TO_TABLEDELTA;
//=======================================================================
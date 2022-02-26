#include "SoundProcessor.h"

//=======================================================================
CSoundProcessor::CSoundProcessor(float fSampleRate)
{
	setSampleRate(fSampleRate);
}

Error_t CSoundProcessor::setSampleRate(float fNewSampleRate)
{
	assert(fNewSampleRate > 0.0);
	if (fNewSampleRate <= 0.0)
		return Error_t::kFunctionInvalidArgsError;

	m_fSampleRateInHz = fNewSampleRate;
	return Error_t::kNoError;
}

float CSoundProcessor::getSampleRate()
{
	return m_fSampleRateInHz;
}

int CSoundProcessor::secToSamp(float sec, float sampleRate) const
{
	return static_cast<int>(sec * sampleRate);
}

float CSoundProcessor::sampToSec(int sample, float sampleRate) const
{
	return static_cast<float>(sample / sampleRate);
}

//=======================================================================

//=======================================================================
CInstrument::CInstrument(float fGain, float fSampleRate) :
	CSoundProcessor(fSampleRate)
{
	setGain(fGain);
	m_adsr.setSampleRate(fSampleRate);
	m_adsr.setParameters(m_adsrParameters);
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

void CInstrument::shiftGain(float fShift)
{
	float fNewGain = m_fGain + fShift;
	assert(fNewGain <= 1 && fNewGain >= -1);
	setGain(fNewGain);
}

Error_t CInstrument::setADSRParameters(float fAttackInSec, float fDecayInSec, float fSustainInSec, float fReleaseInSec)
{
	m_adsrParameters.attack = fAttackInSec;
	m_adsrParameters.decay = fDecayInSec;
	m_adsrParameters.sustain = fSustainInSec;
	m_adsrParameters.release = fReleaseInSec;
	m_adsr.setParameters(m_adsrParameters);
	return Error_t::kNoError;
}

const juce::ADSR::Parameters& CInstrument::getADSRParameters() const
{
	return m_adsrParameters;
}

void CInstrument::resetADSR()
{
	m_adsr.reset();
}

void CInstrument::noteOn()
{
	m_adsr.noteOn();
}

void CInstrument::noteOff()
{
	m_adsr.noteOff();
}

Error_t CInstrument::setSampleRate(float fNewSampleRate)
{
	if (CSoundProcessor::setSampleRate(fNewSampleRate) == Error_t::kNoError)
	{
		m_adsr.setSampleRate(fNewSampleRate);
		m_adsr.setParameters(m_adsrParameters);
		return Error_t::kNoError;
	}
	return Error_t::kFunctionInvalidArgsError;
}
//=======================================================================

//=======================================================================
CWavetableOscillator::CWavetableOscillator(const CWavetable& wavetableToUse, float fFrequencyInHz, float fGain, float fSampleRate) :
	CInstrument(fGain, fSampleRate),
	m_Wavetable(wavetableToUse),
	m_iTableSize(wavetableToUse.getNumSamples())
{
	//assert(wavetableToUse.hasBeenGenerated());
	setFrequency(fFrequencyInHz);
}

Error_t CWavetableOscillator::setFrequency(float fNewFrequencyInHz)
{
	assert(fNewFrequencyInHz >= 0 && fNewFrequencyInHz <= 20000);
	if (fNewFrequencyInHz < 0 || fNewFrequencyInHz > 20000)
		return Error_t::kFunctionInvalidArgsError;
	
	if (m_fSampleRateInHz == 0)
	{
		m_fFrequencyInHz = m_fTableDelta = 0;
	}
	else
	{
		m_fFrequencyInHz = fNewFrequencyInHz;
		m_fTableDelta = (m_fSampleRateInHz == 0.0f) ? 0.0f : (m_iTableSize / m_fSampleRateInHz) * fNewFrequencyInHz;
	}
	return Error_t::kNoError;
}

float CWavetableOscillator::getFrequency() const
{
	return m_fFrequencyInHz;
}

void CWavetableOscillator::shiftFrequency(float fShiftInHz)
{
	float fNewFrequency = m_fFrequencyInHz + fShiftInHz;
	assert(fNewFrequency >= 0);
	setFrequency(fNewFrequency);
}

void CWavetableOscillator::process(float** ppfOutBuffer, int iNumChannels, int currentFrame)
{
	unsigned index0 = (unsigned)m_fCurrentIndex;
	unsigned index1 = index0 + 1;
	if (index1 >= (unsigned)m_iTableSize)
		index1 = (unsigned)0;

	float frac = m_fCurrentIndex - (float)index0;

	const float* wavetableReader = m_Wavetable.getReadPointer(0);
	float value0 = wavetableReader[index0];
	float value1 = wavetableReader[index1];

	float currentSample = value0 + frac * (value1 - value0);

	if ((m_fCurrentIndex += m_fTableDelta) > (float)m_iTableSize)
		m_fCurrentIndex -= (float)m_iTableSize;

	currentSample *= m_adsr.getNextSample() * m_fGain;

	for (int channel = 0; channel < iNumChannels; channel++)
		ppfOutBuffer[channel][currentFrame] += currentSample;
}

Error_t CWavetableOscillator::setSampleRate(float fNewSampleRate)
{
	if (CInstrument::setSampleRate(fNewSampleRate) == Error_t::kNoError)
		return setFrequency(m_fFrequencyInHz);
	return Error_t::kFunctionInvalidArgsError;
}

//=======================================================================
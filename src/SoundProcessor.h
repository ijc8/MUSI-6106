#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <cassert>
#include "ErrorDef.h"
#include "Wavetable.h"

class CSoundProcessor abstract
{
public:
	CSoundProcessor(float fSampleRate);
	virtual ~CSoundProcessor() = default;

	// Call this in the prepareToPlay() function for correct initialization
	// Value must be greater than 0
	virtual Error_t setSampleRate(float fNewSampleRate);

	float getSampleRate();

	
protected:

	int secToSamp(float sec, float sampleRate) const;
	float sampToSec(int sample, float sampleRate) const;
	float m_fSampleRateInHz = 48000.0f;

};

class CInstrument : public CSoundProcessor
{
public:
	CInstrument(float fGain, float fSampleRate);
	virtual ~CInstrument() = default;

	virtual void process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) = 0;

	// Value must be between -1.0 and 1.0 (inclusive)
	Error_t setGain(float fNewGain);

	float getGain() const;

	// Value can be negative or positive
	void shiftGain(float fShift);

	Error_t setADSRParameters(float fAttackInSec, float fDecayInSec, float fSustainLevel, float fReleaseInSec);
	const juce::ADSR::Parameters& getADSRParameters() const;

	// Activates instrument's internal ADSR
	virtual void noteOn();

	// Enters release state of instrument's ADSR
	virtual void noteOff();

	virtual Error_t setSampleRate(float fNewSampleRate) override;

protected:

	float m_fGain = 0.0f;
	juce::ADSR m_adsr;
	juce::ADSR::Parameters m_adsrParameters;

private:

};

class CWavetableOscillator : public CInstrument
{
public:
	CWavetableOscillator(const CWavetable& wavetableToUse, float fFrequencyInHz = 0.0f, float fGain = 0.0f, float fSampleRate = 48000.0f);
	virtual ~CWavetableOscillator() = default;

	// Value must be between 0 and 20000 (inclusive)
	Error_t setFrequency(float fNewFrequencyInHz);

	float getFrequency() const;

	// Value can be negative or positive
	void shiftFrequency(float fShiftInHz);

	Error_t setSampleRate(float fNewSampleRate) override;

	void process(float** ppfOutBuffer, int iNumChannels, int iNumSamples) override;

protected:

	float m_fFrequencyInHz = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fTableDelta = 0.0f;
	const CWavetable& m_Wavetable;
	int m_iTableSize;

};

#endif // #if !defined(__SoundProcessor_hdr__)
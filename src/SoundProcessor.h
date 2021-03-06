#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <juce_audio_basics/juce_audio_basics.h>

#include <cassert>
#include "ErrorDef.h"

class CSoundProcessor
{
public:
	CSoundProcessor(float fSampleRate);
	virtual ~CSoundProcessor() = default;

	// Call this in the prepareToPlay() function for correct initialization
	// Value must be greater than 0
	virtual Error_t setSampleRate(float fNewSampleRate);

	float getSampleRate();

	// This is a flexible process function that should cover all current and future needs
	// Most classes implement this as frame-by-frame processing
	virtual void processFrame(float**, int, int) = 0;

	
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


	// Value must be between -1.0 and 1.0 (inclusive)
	Error_t setGain(float fNewGain);

	float getGain() const;

    // Set Pan
    Error_t setPan(float fPan);

    float getPan() const;

	// Value can be negative or positive
	Error_t shiftGain(float fShift);

	Error_t setADSRParameters(float fAttackInSec, float fDecayInSec, float fSustainLevel, float fReleaseInSec);
	const juce::ADSR::Parameters& getADSRParameters() const;

	// Reset all parameters and variables to initial states
	virtual void reset();

	// Resets ADSR regardless of current state
	void resetADSR();

    // Check active oscillators
    bool isActive() const;

	// Activates instrument's internal ADSR
	virtual void noteOn(bool shouldReset = true);

	// Enters release state of instrument's ADSR
	virtual void noteOff();

	// Overrides to reinitialize ADSR valuesS
	virtual Error_t setSampleRate(float fNewSampleRate) override;

protected:

	float m_fGain = 0.0f;
    float m_fPan = 0.5f;
    juce::ADSR m_adsr;
	juce::ADSR::Parameters m_adsrParameters;
	std::atomic<bool> m_bNoteOnPressed = false;
	std::atomic<bool> m_bNoteOffPressed = false;
	bool mShouldReset = true;

	virtual void checkFlags();

private:

};

class CWavetableOscillator : public CInstrument
{
public:
	CWavetableOscillator(const juce::AudioSampleBuffer &wavetableToUse, float fFrequencyInHz = 0.0f, float fGain = 0.0f, float fSampleRate = 48000.0f);
	CWavetableOscillator(const CWavetableOscillator& other);
	virtual ~CWavetableOscillator() = default;

	// Value must be between 0 and 20000 (inclusive)
	Error_t setFrequency(float fNewFrequencyInHz);

	float getFrequency() const;

	// Value can be negative or positive
	Error_t shiftFrequency(float fShiftInHz);

	void reset() override;

	// Overrides because frequency must be reinitialized
	Error_t setSampleRate(float fNewSampleRate) override;

	// Processes frame-by-frame
	void processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;

protected:

	float m_fFrequencyInHz = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fTableDelta = 0.0f;
	const juce::AudioSampleBuffer &wavetable;
	int m_iTableSize;

};

#endif // #if !defined(__SoundProcessor_hdr__)

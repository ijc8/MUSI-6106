#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <cassert>
#include "ErrorDef.h"
#include "Wavetable.h"

class CSoundProcessor
{
public:
	CSoundProcessor(float fSampleRate);
	virtual ~CSoundProcessor() = default;

	virtual float process() = 0;
	virtual void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock) = 0;

	virtual Error_t setSampleRate(float fNewSampleRate);
	float getSampleRate();

	
protected:

	float m_fSampleRateInHz = 48000.0f;

};

class CInstrument : public CSoundProcessor
{
	friend class Scheduler;
public:
	CInstrument(float fGain, float fSampleRate);
	virtual ~CInstrument() = default;

	Error_t setGain(float fNewGain);
	float getGain() const;
	void shiftGain(float fShift);

	Error_t setADSRParameters(float fAttack, float fDecay, float fSustain, float fRelease);
	const juce::ADSR::Parameters& getADSRParameters() const;
	virtual void noteOn();
	void noteOff();

	virtual Error_t setSampleRate(float fNewSampleRate) override;

protected:

	float m_fGain = 0.0f;
	juce::ADSR m_adsr;
	juce::ADSR::Parameters m_adsrParameters;
	int noteOnSample = 0;
	int noteOffSample = 0;
	bool hasBeenScheduled = false;

private:

	void schedule(int noteOn, int noteOff)
	{
		noteOnSample = noteOn;
		noteOffSample = noteOff;
		hasBeenScheduled = true;
	};
};

class CWavetableOscillator : public CInstrument
{
public:
	CWavetableOscillator(const CWavetable& wavetableToUse, float fFrequency = 0.0f, float fGain = 0.0f, float fSampleRate = 48000.0f);
	virtual ~CWavetableOscillator() = default;

	Error_t setFrequency(float fNewFrequency);
	float getFrequency() const;
	void shiftFrequency(float fShift);

	Error_t setSampleRate(float fNewSampleRate) override;
	float process() override;
	void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock) override;

protected:

	float m_fFrequencyInHz = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fTableDelta = 0.0f;
	const CWavetable& m_Wavetable;
	int m_iTableSize;

};

#endif // #if !defined(__SoundProcessor_hdr__)
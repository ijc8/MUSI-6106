#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <cassert>
#include "ErrorDef.h"
#include "Wavetable.h"

class CSoundProcessor
{
public:
	CSoundProcessor();
	virtual ~CSoundProcessor();

	static Error_t setSampleRate(float fNewSampleRate);
	static float getSampleRate();

	virtual float process() = 0;
	virtual void reinitialize() = 0;
	
protected:

	static float s_fSampleRateInHz;

};

class CInstrument : public CSoundProcessor
{
public:
	CInstrument();
	virtual ~CInstrument();

	Error_t setGain(float fNewGain);
	float getGain() const;

protected:

	float m_fGain;
};

class CWavetableOscillator : public CInstrument
{
public:
	CWavetableOscillator(const Wavetable& wavetableToUse, float fFrequency = 440.0f, float fGain = 1.0f);
	virtual ~CWavetableOscillator();
	static Error_t updateConversionFactors();

	Error_t setFrequency(float fNewFrequency);
	float getFrequency() const;
	float process() override;
	void reinitialize() override;

protected:

	static float s_FREQ_TO_TABLEDELTA;
	static float s_TABLEDELTA_TO_FREQ;

	float m_fFrequencyInHz;
	float m_fCurrentIndex;
	float m_fTableDelta;
	const Wavetable& m_Wavetable;

};

#endif // #if !defined(__SoundProcessor_hdr__)
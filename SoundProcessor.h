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

	/* Sets sample rate for all sound processors. 
	*  Requires followup call to reinitialize()
	\return Error_t
	*/
	static Error_t setSampleRate(float fNewSampleRate);

	/* Returns current sample rate for sound processors.
	\return float
	*/
	static float getSampleRate();

	/* Override this for audio processing.
	\return float
	*/
	virtual float process() = 0;

	/* Override this to recalculate member variables that depend on sampling rate. 
	\return void
	*/
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

	/* Outputs values from selected wavetable.
	\return float
	*/
	float process() override;

	/* Resets frequency using proper sample rate 
	\return void 
	*/
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
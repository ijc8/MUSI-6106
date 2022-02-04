#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <cassert>
#include "ErrorDef.h"

class CSoundProcessor
{
public:
	CSoundProcessor();
	virtual ~CSoundProcessor();

	static Error_t setSampleRate(float fNewSampleRate);

	virtual float process() = 0;
	Error_t setGain(float fNewGain);
	float getGain() const;
	
protected:

	static float s_fSampleRateInHz;
	float m_fGain;

private:

};

class COscillator : public CSoundProcessor
{
public:
	COscillator(float fFrequency);
	virtual ~COscillator();

	static Error_t create(CSoundProcessor*& pCSoundProcessor, float fFrequency);
	static Error_t destroy(CSoundProcessor*& pCSoundProcessor);

	Error_t setFrequency(float fNewFrequency);
	float getFrequency() const;
	float process() override;

protected:

	float m_fFrequency;

private:

};

#endif // #if !defined(__SoundProcessor_hdr__)
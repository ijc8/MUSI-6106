#if !defined(__Wavetable_hdr__)
#define __Wavetable_hdr__

#include <juce_audio_processors/juce_audio_processors.h>

class CWavetable
{
public:
    CWavetable() {};
	virtual ~CWavetable() {};

    const float* getReadPointer(int sampleIndex = 0) const { return m_fWavetable.getReadPointer(0, sampleIndex); };
	virtual void createWavetable() = 0;
    static int getNumSamples() { return s_iTableSize; };

protected:

	static const unsigned s_iTableSize = 1 << 9;
    juce::AudioSampleBuffer m_fWavetable;

};

class CSineWavetable : public CWavetable
{
public:

    CSineWavetable() {};
    ~CSineWavetable() {};

	void createWavetable() override
	{
        m_fWavetable.setSize(1, (int)s_iTableSize);
        float* samples = m_fWavetable.getWritePointer(0);     

        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(s_iTableSize - 1); 
        auto currentAngle = 0.0;

        for (unsigned int i = 0; i < s_iTableSize; ++i)
        {
            auto sample = std::sin(currentAngle);                                       
            samples[i] = (float)sample;
            currentAngle += angleDelta;
        }
	}

};


#endif // #if !defined(__Wavetable_hdr__)
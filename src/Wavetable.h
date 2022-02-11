#if !defined(__Wavetable_hdr__)
#define __Wavetable_hdr__

#include <juce_audio_processors/juce_audio_processors.h>

class CWavetable
{
public:
    CWavetable() {};
	virtual ~CWavetable() {};

    const float* getReadPointer(int sampleIndex = 0) const { return m_fWavetable.getReadPointer(0, sampleIndex); };
	virtual void generateWavetable() = 0;
    int getNumSamples() const { return s_iTableSize; };
    bool hasBeenGenerated() const { return m_bHasBeenGenerated; };


protected:

	const unsigned s_iTableSize = 1 << 9;
    juce::AudioSampleBuffer m_fWavetable;
    bool m_bHasBeenGenerated = false;

};

class CSineWavetable : public CWavetable
{
public:

    CSineWavetable() {};
    ~CSineWavetable() {};

	void generateWavetable() override
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

        m_bHasBeenGenerated = true;
	}

};


#endif // #if !defined(__Wavetable_hdr__)
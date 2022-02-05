#if !defined(__Wavetable_hdr__)
#define __Wavetable_hdr__

#include <juce_audio_processors/juce_audio_processors.h>

class Wavetable
{
public:
    Wavetable() {};
	virtual ~Wavetable() {};

	virtual void createWavetable() = 0;
    static int getNumSamples() { return s_iTableSize; };

protected:

	static const unsigned s_iTableSize = 1 << 7;
    juce::AudioSampleBuffer m_fWavetable;

};

class SineWavetable : public Wavetable
{
public:

    SineWavetable() {};
    ~SineWavetable() {};

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

private:
};


#endif // #if !defined(__Wavetable_hdr__)
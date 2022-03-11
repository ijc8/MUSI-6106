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

	const int s_iTableSize = 1 << 9;
    juce::AudioSampleBuffer m_fWavetable;
    bool m_bHasBeenGenerated = false;

};

class CSineWavetable : public CWavetable
{
public:

    CSineWavetable() {generateWavetable();}
    ~CSineWavetable() {};

	void generateWavetable() override
	{
        m_fWavetable.setSize(1, (int)s_iTableSize);
        float* samples = m_fWavetable.getWritePointer(0);     
        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(s_iTableSize); 
        auto currentAngle = 0.0;

        for (int i = 0; i < s_iTableSize; ++i)
        {
            auto sample = std::sin(currentAngle);                                       
            samples[i] = (float)sample;
            currentAngle += angleDelta;
        }

        m_bHasBeenGenerated = true;
	}

};

class CSawWavetable : public CWavetable {
public:

    CSawWavetable() {generateWavetable();};
    ~CSawWavetable() {};

    void generateWavetable() override{
        m_fWavetable.setSize(1, (int) s_iTableSize);
        float *samples = m_fWavetable.getWritePointer(0);
        int numRampUp = s_iTableSize;
        for (int i = 0; i < s_iTableSize; ++i) {
            samples[i] = 1.f / static_cast<float>(numRampUp) * static_cast<float>(i);
            samples[i] = samples[i] * 2 - 1;
        }
        m_bHasBeenGenerated = true;
    }
};


class CTriWavetable : public CWavetable {
public:

    CTriWavetable() {generateWavetable();};
    ~CTriWavetable() {};

    void generateWavetable() override{
        m_fWavetable.setSize(1, (int) s_iTableSize);
        float *samples = m_fWavetable.getWritePointer(0);
        int numRampUp = s_iTableSize / 2 + 1;
        for (int i = 0; i < s_iTableSize; ++i) {
            samples[i] = i < numRampUp ? 1.f / static_cast<float>(numRampUp) * static_cast<float>(i) :
                              2.f - 1.f / static_cast<float>(numRampUp) * static_cast<float>(i);
            samples[i] = samples[i] * 2 - 1;
        }
        m_bHasBeenGenerated = true;
    }
};


class CSqrWavetable : public CWavetable {
public:

    CSqrWavetable() {generateWavetable();};
    ~CSqrWavetable() {};

    void generateWavetable() override{
        m_fWavetable.setSize(1, (int) s_iTableSize);
        float *samples = m_fWavetable.getWritePointer(0);
        for (int i=0; i<s_iTableSize; ++i)
            if (i < s_iTableSize / 2)
                samples[i] = -1.f;
            else
                samples[i] = 1.f;

        m_bHasBeenGenerated = true;
    }
};


#endif // #if !defined(__Wavetable_hdr__)
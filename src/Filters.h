#ifndef MUSICAL_CHESS_FILTERS_H
#define MUSICAL_CHESS_FILTERS_H

#pragma once
#include <juce_dsp/juce_dsp.h>

class Filters
{
public:
    enum class Type
    {
        lowPass,
        bandPass,
        highPass,
    };

    Filters();

    ~Filters();

    void reset();

    void process(juce::AudioBuffer<float>& buffer);

    void setType(Type);

    Type getType() const;

    void setCutoffFrequency(float freqInHz);

    float getCutFrequency() const;

    void setResonance(float res);

    float getResonance() const;

    void setBypass(bool isBypassed);

    bool getBypass() const;


private:

    struct FilterSpec{

        std::atomic<bool> isBypassed;

    } svfSpec;

    juce::dsp::StateVariableTPTFilter<float> *pSVF;

    bool isInitialized;

    juce::dsp::ProcessSpec processSpec;

};

#endif //MUSICAL_CHESS_FILTERS_H

#include "Filters.h"

Filters::Filters(): pSVF(nullptr), isInitialized(false), processSpec({0, 0, 0})
{
    svfSpec.isBypassed = false;
    pSVF = new juce::dsp::StateVariableTPTFilter<float>;
    pSVF->prepare(processSpec);
}

Filters::~Filters()
{
    reset();
}


void Filters::reset()
{
    delete pSVF;
    pSVF = nullptr;

    processSpec = {0.0, 0, 0};
    svfSpec.isBypassed = false;

    isInitialized = false;
}

void Filters::setType(Type type)
{
    if (!isInitialized)
        return;

    switch (type)
    {
        case Type::lowPass:
            pSVF->setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
        case Type::bandPass:
            pSVF->setType(juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
        case Type::highPass:
            pSVF->setType(juce::dsp::StateVariableTPTFilterType::highpass);
            break;
    }
}

Filters::Type Filters::getType() const
{
    if (!isInitialized)
        return Type::lowPass;

    auto internalType = pSVF->getType();

    switch(internalType)
    {
        case juce::dsp::StateVariableTPTFilterType::lowpass:
            return Type::lowPass;
        case juce::dsp::StateVariableTPTFilterType::bandpass:
            return Type::bandPass;
        case juce::dsp::StateVariableTPTFilterType::highpass:
            return Type::highPass;
    }
}

void Filters::setCutoffFrequency(float freqInHz)
{
    if (!isInitialized)
        return;

    pSVF->setCutoffFrequency(freqInHz);
}

float Filters::getCutFrequency() const
{
    if (!isInitialized)
        return 0.f;

    return pSVF->getCutoffFrequency();
}

void Filters::setResonance(float res)
{
    if (!isInitialized)
        return;

    pSVF->setResonance(res);
}

float Filters::getResonance() const
{
    if (!isInitialized)
        return 0.f;

    return pSVF->getResonance();
}

void Filters::setBypass(bool isBypassed)
{
    svfSpec.isBypassed = isBypassed;
}

bool Filters::getBypass() const
{
    return svfSpec.isBypassed;
}

void Filters::process(juce::AudioBuffer<float>& buffer)
{
    if (svfSpec.isBypassed)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    pSVF->process(context);

}
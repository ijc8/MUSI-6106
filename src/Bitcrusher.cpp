//
// Created by Rose Sun on 4/6/22.
//

#include "Bitcrusher.h"

Bitcrusher::Bitcrusher(): ppDownSampleInterp(nullptr), ppHoldInterp(nullptr), isInitialized(false), processSpec({0, 0, 0})
{
    bitcrusherSpec.freqInHz = 0.f; bitcrusherSpec.bitDepth = 0.f;
    bitcrusherSpec.isDithering = false; bitcrusherSpec.mix = 0.f;
}

Bitcrusher::~Bitcrusher()
{
    reset();
}

bool Bitcrusher::initialize(const juce::dsp::ProcessSpec &spec)
{

    dryBuffer.setSize(processSpec.numChannels, processSpec.maximumBlockSize);
    interpBuffer.setSize(processSpec.numChannels, processSpec.maximumBlockSize);

    ppDownSampleInterp = new juce::CatmullRomInterpolator*[processSpec.numChannels];
    for (int c = 0; c < processSpec.numChannels; ++c)
        ppDownSampleInterp[c] = new juce::CatmullRomInterpolator;

    ppHoldInterp = new juce::ZeroOrderHoldInterpolator*[processSpec.numChannels];
    for (int c = 0; c < processSpec.numChannels; ++c)
        ppHoldInterp[c] = new juce::ZeroOrderHoldInterpolator;

    return true;
}

void Bitcrusher::reset()
{
    if (!isInitialized)
        return;

    for (int c = 0; c < processSpec.numChannels; ++c)
    {
        delete ppHoldInterp[c];
        delete ppDownSampleInterp[c];
    }
    delete[] ppDownSampleInterp; ppDownSampleInterp = nullptr;
    delete[] ppHoldInterp; ppHoldInterp = nullptr;

    processSpec = {0.0, 0, 0};
    bitcrusherSpec.freqInHz = 0.f; bitcrusherSpec.bitDepth = 0.f;
    bitcrusherSpec.isDithering = false; bitcrusherSpec.mix = 0.f;
    isInitialized = false;
}

void Bitcrusher::clear()
{
    if (!isInitialized)
        return;

    dryBuffer.clear();
    interpBuffer.clear();
    for (int c = 0; c < processSpec.numChannels; ++c)
    {
        ppDownSampleInterp[c]->reset();
        ppHoldInterp[c]->reset();
    }
}

void Bitcrusher::process(juce::AudioBuffer<float> &buffer)
{
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
    auto ppBuffer = buffer.getArrayOfWritePointers();
    auto ppInterp = interpBuffer.getArrayOfWritePointers();

    for (int c = 0; c < numChannels; ++c)
        dryBuffer.copyFrom(c, 0, buffer, c, 0, numSamples);

    // 1. down-sampling fx
    if (bitcrusherSpec.freqInHz < processSpec.sampleRate)
    {
        float freqInHz = bitcrusherSpec.freqInHz;
        float ratio = processSpec.sampleRate / freqInHz;
        int numInterpSamples = static_cast<int>(round(1.f / ratio * numSamples));

        for (int c = 0; c < numChannels; ++c)
        {
            ppDownSampleInterp[c]->process(ratio, ppBuffer[c], ppInterp[c], numInterpSamples);
            ppHoldInterp[c]->process(1.f / ratio, ppInterp[c], ppBuffer[c], numSamples);
        }
    }

    // 2. quantization fx
    float amp;
    bool isDithering = bitcrusherSpec.isDithering;
    float noise;
    for (int i=0; i<numSamples; ++i)
    {
        if (isDithering)
            noise = ((rand.nextFloat() + rand.nextFloat()) - 1.f) / 0.5f;
        else
            noise = 0;
        amp = std::pow(2.f, bitcrusherSpec.bitDepth-1.f);
        for (int c=0; c<numChannels; ++c)
            ppBuffer[c][i] = round(ppBuffer[c][i] * amp + noise) / amp;
    }

    // 3. mix dry and wet
    float mix = bitcrusherSpec.mix;
    dryBuffer.applyGain(1.f - mix);
    buffer.applyGain(mix);
    for (int c = 0; c < numChannels; ++c)
        buffer.addFrom(c, 0, dryBuffer, c, 0, numSamples);
}

void Bitcrusher::setClockFrequency(float freqInHz)
{
    if (!isInitialized)
        return;

    if (freqInHz < 0.f)
        freqInHz = 0.f;

    bitcrusherSpec.freqInHz = freqInHz;
}

float Bitcrusher::getClockFrequency() const
{
    return bitcrusherSpec.freqInHz;
}

void Bitcrusher::setBitDepth(float depth)
{
    if (!isInitialized)
        return;

    if (depth < 1.f)
        depth = 1.f;

    bitcrusherSpec.bitDepth = depth;
}

float Bitcrusher::getBitDepth() const
{
    return bitcrusherSpec.bitDepth;
}

void Bitcrusher::setDither(bool isDithering)
{
    if (!isInitialized)
        return;

    bitcrusherSpec.isDithering = isDithering;
}

bool Bitcrusher::getDither() const
{
    return bitcrusherSpec.isDithering;
}

void Bitcrusher::setMix(float value)
{
    if (!isInitialized)
        return;

    if (value > 1.f)
        value = 1.f;
    else if (value < 0.f)
        value = 0.f;

    bitcrusherSpec.mix = value;
}

float Bitcrusher::getMix() const
{
    return bitcrusherSpec.mix;
}
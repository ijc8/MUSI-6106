//
// Created by Rose Sun on 4/6/22.
//

#ifndef MUSICAL_CHESS_BITCRUSHER_H
#define MUSICAL_CHESS_BITCRUSHER_H

#pragma once
#include <juce_dsp/juce_dsp.h>

class Bitcrusher
{
public:
    Bitcrusher();
    ~Bitcrusher();

    bool initialize(const juce::dsp::ProcessSpec& spec);
    void reset();
    void clear();
    void process(juce::AudioBuffer<float> &buffer);

    void setClockFrequency(float freqInHz);

    float getClockFrequency() const;

    void setBitDepth(float depth);

    float getBitDepth() const;

    void setDither(bool isDithering);

    bool getDither() const;

    void setMix(float value);

    float getMix() const;

private:
    struct BitcrusherSpec
    {
        std::atomic<float> freqInHz=0.f; std::atomic<float> bitDepth=0.f; std::atomic<bool> isDithering; std::atomic<float> mix;
    } bitcrusherSpec;

    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> interpBuffer;

    juce::Random rand;

    juce::CatmullRomInterpolator** ppDownSampleInterp;
    juce::ZeroOrderHoldInterpolator** ppHoldInterp;

    bool isInitialized;
    juce::dsp::ProcessSpec processSpec;
};

#endif //MUSICAL_CHESS_BITCRUSHER_H

#include "Waveform.h"

juce::AudioSampleBuffer Waveform::generateSine(int size) {
    juce::AudioSampleBuffer waveform;
    waveform.setSize(1, (int)size);
    float *samples = waveform.getWritePointer(0);
    auto angleDelta = juce::MathConstants<double>::twoPi / (double)(size);
    auto currentAngle = 0.0;

    for (int i = 0; i < size; i++) {
        auto sample = std::sin(currentAngle);
        samples[i] = (float)sample;
        currentAngle += angleDelta;
    }
    return waveform;
}

juce::AudioSampleBuffer Waveform::generateSaw(int size) {
    juce::AudioSampleBuffer waveform;
    waveform.setSize(1, (int)size);
    float *samples = waveform.getWritePointer(0);
    for (int i = 0; i < size; i++) {
        samples[i] = 1.f / static_cast<float>(size) * static_cast<float>(i);
        samples[i] = samples[i] * 2 - 1;
    }
    return waveform;
}

juce::AudioSampleBuffer Waveform::generateTri(int size) {
    juce::AudioSampleBuffer waveform;
    waveform.setSize(1, (int)size);
    float *samples = waveform.getWritePointer(0);
    int numRampUp = size / 2 + 1;
    for (int i = 0; i < size; ++i) {
        samples[i] = i < numRampUp ? 1.f / static_cast<float>(numRampUp) * static_cast<float>(i) : 2.f - 1.f / static_cast<float>(numRampUp) * static_cast<float>(i);
        samples[i] = samples[i] * 2 - 1;
    }
    return waveform;
}

juce::AudioSampleBuffer Waveform::generateSqr(int size) {
    juce::AudioSampleBuffer waveform;
    waveform.setSize(1, (int)size);
    float *samples = waveform.getWritePointer(0);
    for (int i = 0; i < size; ++i) {
        samples[i] = i < size / 2 ? -1.f : 1.f;
    }
    return waveform;
}

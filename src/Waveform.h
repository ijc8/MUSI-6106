#if !defined(__Waveform_hdr__)
#define __Waveform_hdr__

#include <juce_audio_processors/juce_audio_processors.h>

namespace Waveform {
    constexpr int SIZE = 1 << 9;
    juce::AudioSampleBuffer generateSine(int size = SIZE);
    juce::AudioSampleBuffer generateSaw(int size = SIZE);
    juce::AudioSampleBuffer generateTri(int size = SIZE);
    juce::AudioSampleBuffer generateSqr(int size = SIZE);

    const juce::AudioSampleBuffer sine = generateSine();
    const juce::AudioSampleBuffer saw = generateSaw();
    const juce::AudioSampleBuffer tri = generateTri();
    const juce::AudioSampleBuffer sqr = generateSqr();
}

#endif // #if !defined(__Waveform_hdr__)

#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "../../src/MainProcessor.h"
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    float mSampleRate = 0.0f;

    CSineWavetable sine;

    MainProcessor mainProcessor;
    CWavetableOscillator pawnOsc = CWavetableOscillator(sine, 343, 1);
    CWavetableOscillator knightOsc = CWavetableOscillator(sine, 170, 1);
    CWavetableOscillator queenOsc = CWavetableOscillator(sine, 900, 0.5);
    CWavetableOscillator kingOsc = CWavetableOscillator(sine, 55, 0.25);
    Looper loop;

    CSoundProcessor* processors[6]
    {
        &mainProcessor,
        &pawnOsc,
        &knightOsc,
        &queenOsc,
        &kingOsc,
        &loop
    };

    CInstrument* instruments[5]
    {
        &pawnOsc,
        &knightOsc,
        &queenOsc,
        &kingOsc,
        &loop
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

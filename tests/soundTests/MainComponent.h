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

    juce::TextButton loopButton;
    juce::TextButton increaseFreqButton;
    juce::TextButton oscButton;
    juce::TextButton pawnButton;
    juce::TextButton loopButton1;

    ////////////////////////////////////////////////////
    /// Synthesis stuff

    CSineWavetable sine;
    CMainProcessor mainProcessor;
    CWavetableOscillator pawnOsc = CWavetableOscillator(sine, 343, 1);
    CLooper loop;
    CScheduler schedule;

    CSoundProcessor* processors[4]
    {
        &mainProcessor,
        &schedule,
        &pawnOsc,
        &loop
    };

    ////////////////////////////////////////////////////

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

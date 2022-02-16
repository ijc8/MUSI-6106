#include "MainComponent.h"
#include "Scheduler.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    const float fSampleRate = 44100;

    CSineWavetable sine;
    sine.generateWavetable();

    CScheduler schedule(fSampleRate);
    CInstrument* osc1 = new CWavetableOscillator(sine, 440, 1, fSampleRate);
    CInstrument* osc2 = new CWavetableOscillator(sine, 220, 1, fSampleRate);

    schedule.add(osc1, 1, 1);
    schedule.add(osc2, 2, 1);
    schedule.add(osc1, 3, 3);

    delete osc1;
    delete osc2;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

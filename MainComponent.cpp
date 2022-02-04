#include "MainComponent.h"
#include "SoundProcessor.h"
#include "Wavetable.h"
#include "ErrorDef.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    CSoundProcessor::setSampleRate(44100.0f);
    CSoundProcessor* pCSoundProcessor = 0;
    COscillator::create(pCSoundProcessor, 440.0f);
    pCSoundProcessor->process();
    COscillator::destroy(pCSoundProcessor);

    SineWavetable sine;
    sine.createWavetable();
   
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

#include "MainComponent.h"
#include "SoundProcessor.h"

#include <fstream>

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    CSoundProcessor::setSampleRate(48000.0f);
    SineWavetable sine;
    sine.createWavetable();

    std::ofstream out_file;
    out_file.open("C:/Users/JohnK/Documents/CS/MusicalChess/out.txt");

    CInstrument* pCInstrument = new CWavetableOscillator(sine, 440.0f, 0.5f);

    int iNumSamples = 100000;
    for (int sample{ 0 }; sample < iNumSamples - 1; sample++)
    {
        out_file << pCInstrument->process() << ",";
        CSoundProcessor::setSampleRate(48000.0f + sample);
        pCInstrument->reinitialize();
    }
    out_file << pCInstrument->process();
    out_file << std::endl;
    std::cout << "done" << std::endl;

    delete pCInstrument;
    out_file.close();
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

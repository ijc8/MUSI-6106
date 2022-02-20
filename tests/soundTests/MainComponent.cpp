#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }


}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mSampleRate = sampleRate;
    mainProcessor.setSampleRate(sampleRate);
    pawnOsc.setSampleRate(sampleRate);
    loop1.setSampleRate(sampleRate);

    loop1.pushInst(new CWavetableOscillator(sine, 230, 1, sampleRate), 0.5, 0);
    loop1.pushInst(new CWavetableOscillator(sine, 460, 1, sampleRate), 0.5, 0.5);
    loop1.pushInst(new CWavetableOscillator(sine, 600, 1, sampleRate), 0.5, 1);

    mainProcessor.addInstRef(pawnOsc);
    mainProcessor.addInstRef(loop1);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    static int totalSample = 0;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    bufferToFill.clearActiveBufferRegion();

    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        float fCurrentSample = mainProcessor.process();
        leftChannel[sample] += fCurrentSample;
        rightChannel[sample] += fCurrentSample;


        if (totalSample == 48000)
            pawnOsc.noteOn();
        if (totalSample == 300000)
            pawnOsc.noteOff();
        if (totalSample == 200000)
        {
            loop1.noteOn();
        }

        totalSample++;
    }

}

void MainComponent::releaseResources()
{

}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

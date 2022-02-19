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

}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    static int totalSample = 0;
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    bufferToFill.clearActiveBufferRegion();

    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        float fCurrentSample = 0;
        for (Looper* looper : mainProcessor)
            fCurrentSample += looper->process();
        leftChannel[sample] += fCurrentSample;
        rightChannel[sample] += fCurrentSample;


        if (totalSample == 100000) {
            Looper* looper = new Looper(48000);
            looper->pushInst(new CWavetableOscillator(sine, 440, 1, 48000), 1, 0);
            looper->pushInst(new CWavetableOscillator(sine, 220, 1, 48000), 1, 1);
            looper->setLoopLength(4);
            mainProcessor.push_back(looper);
        }
        totalSample++;

    }

}

void MainComponent::releaseResources()
{
    for (Looper* looper : mainProcessor)
        delete looper;
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

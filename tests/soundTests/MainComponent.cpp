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
    for (auto& processor : processors)
        processor->setSampleRate(sampleRate);

    //Scheduler* schedule = new Scheduler(sampleRate);

    //schedule->pushInst(new CWavetableOscillator(sine, 110, 1, sampleRate), 0.5, 0);
    //schedule->pushInst(new CWavetableOscillator(sine, 130.81, 1, sampleRate), 0.5, 0.5);
    //schedule->pushInst(new CWavetableOscillator(sine, 164.81, 1, sampleRate), 0.5, 1);
    //mainProcessor.pushInst(schedule);

    loop.pushInst(new CWavetableOscillator(sine, 110, 1, sampleRate), 0.5, 0);
    loop.pushInst(new CWavetableOscillator(sine, 130.81, 1, sampleRate), 0.5, 0.5);
    loop.pushInst(new CWavetableOscillator(sine, 164.81, 1, sampleRate), 0.5, 1);
    mainProcessor.addInstRef(loop);

    loop.setADSRParameters(4, .5, 1, 2);
    pawnOsc.setADSRParameters(1, .3, 0.5, 4);
    knightOsc.setADSRParameters(0.5, 0.1, 0.75, 3);

    mainProcessor.addInstRef(pawnOsc);
    mainProcessor.addInstRef(knightOsc);
    mainProcessor.addInstRef(queenOsc);
    mainProcessor.addInstRef(kingOsc);
    mainProcessor.addInstRef(loop);

    for (auto instrument : instruments)
        instrument->noteOn();

    mainProcessor.pushInst(new CWavetableOscillator(sine, 560, 1, sampleRate), 4, 0);
}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    //static int totalSample = 0;
    //float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    //float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    //bufferToFill.clearActiveBufferRegion();

    //for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    //{
    //    float fCurrentSample = mainProcessor.process();
    //    leftChannel[sample] += fCurrentSample;
    //    rightChannel[sample] += fCurrentSample;
    //}

    mainProcessor.process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples, 0);

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

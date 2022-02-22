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

    addAndMakeVisible(loopButton);
    loopButton.setButtonText("Add Loop");
    loopButton.onClick = [this]() {
        CLooper* newLoop = new CLooper(mSampleRate);
        newLoop->pushInst(new CWavetableOscillator(sine, 110, 1, mSampleRate), 0, 0.5);
        newLoop->pushInst(new CWavetableOscillator(sine, 130.81, 1, mSampleRate), 0.5, 0.5);
        newLoop->pushInst(new CWavetableOscillator(sine, 164.81, 1, mSampleRate), 1, 0.5);
        mainProcessor.pushLooper(newLoop, 3);
    };

    addAndMakeVisible(increaseFreqButton);
    increaseFreqButton.setButtonText("Increase Freq");
    increaseFreqButton.onClick = [this]() {
        pawnOsc.shiftFrequency(50);
    };

    addAndMakeVisible(oscButton);
    oscButton.setButtonText("Add Oscillator");
    oscButton.onClick = [this]() {
        CWavetableOscillator* newOsc = new CWavetableOscillator(sine, 800, 1, mSampleRate);
        newOsc->setADSRParameters(3, .3, 0.5, 3);
        mainProcessor.pushInst(newOsc, 0, 5);
    };

    addAndMakeVisible(pawnButton);
    pawnButton.setButtonText("Pawn");
    pawnButton.setClickingTogglesState(true);
    pawnButton.onClick = [this]() {
        if (pawnButton.getToggleState())
            pawnOsc.noteOn();
        else
            pawnOsc.noteOff();
    };

    addAndMakeVisible(loopButton1);
    loopButton1.setButtonText("Loop");
    loopButton1.setClickingTogglesState(true);
    loopButton1.onClick = [this]() {
        if (loopButton1.getToggleState())
            loop.start();
        else
            loop.stop();
    };


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

    mainProcessor.addInstRef(pawnOsc);

    loop.pushInst(new CWavetableOscillator(sine, 220, 1, mSampleRate), 0, 0.5);
    loop.pushInst(new CWavetableOscillator(sine, 260, 1, mSampleRate), 0.5, 0.5);
    loop.pushInst(new CWavetableOscillator(sine, 328, 1, mSampleRate), 1, 0.5);
    loop.setLoopLength(3);
    mainProcessor.addScheduleRef(loop);
}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

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
    auto area = getBounds().reduced(10);

    const int numButtons = 5;

    loopButton.setBounds(area.removeFromTop(getHeight()/ numButtons));
    increaseFreqButton.setBounds(area.removeFromTop(getHeight() / numButtons));
    oscButton.setBounds(area.removeFromTop(getHeight() / numButtons));
    pawnButton.setBounds(area.removeFromTop(getHeight() / numButtons));
    loopButton1.setBounds(area.removeFromTop(getHeight() / numButtons));

}

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
    // text buttons
    addAndMakeVisible(Board1);
    Board1.setButtonText("8/8/8/4p1K1/2k1P3/8/8/8");
    Board1.onClick = [this](){
        m_Board.setBoardFen("8/8/8/4p1K1/2k1P3/8/8/8");
        m_DebugSonifier.onMove(m_Board);
    };

    addAndMakeVisible(Board2);
    Board2.setButtonText("4k2r/6r1/8/8/8/8/3R4/R3K3");
    Board2.onClick = [this](){
        m_Board.setBoardFen("4k2r/6r1/8/8/8/8/3R4/R3K3");
        m_DebugSonifier.onMove(m_Board);
    };

    addAndMakeVisible(Board3);
    Board3.setButtonText("8/8/8/8/8/8/8/8");
    Board3.onClick = [this](){
        m_Board.setBoardFen("8/8/8/8/8/8/8/8");
        m_DebugSonifier.onMove(m_Board);
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
    m_DebugSonifier.prepareToPlay(samplesPerBlockExpected,sampleRate);
    m_DebugSonifier.onMove(m_Board);
}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    m_DebugSonifier.process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.buffer->getNumSamples());
}

void MainComponent::releaseResources()
{
    m_DebugSonifier.releaseResources();
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

    const int numButtons = 3;

    Board1.setBounds(area.removeFromTop(getHeight()/ numButtons));
    Board2.setBounds(area.removeFromTop(getHeight() / numButtons));
    Board3.setBounds(area.removeFromTop(getHeight() / numButtons));

}

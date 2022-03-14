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
    Board1.setButtonText("rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/2N2N2/PPPP1PPP/R1BQK2R");
    Board1.onClick = [this](){
        m_GameState.setBoardFen("rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/2N2N2/PPPP1PPP/R1BQK2R");
        m_ThreatsSonifier.onMove(m_GameState);
    };

    addAndMakeVisible(Board2);
    Board2.setButtonText("3r1rk1/ppp1qpp1/3p1n1p/2b1p3/2BnP3/2NP1N2/PPP1QPPP/R1B2RK1");
    Board2.onClick = [this](){
        m_GameState.setBoardFen("3r1rk1/ppp1qpp1/3p1n1p/2b1p3/2BnP3/2NP1N2/PPP1QPPP/R1B2RK1");
        m_ThreatsSonifier.onMove(m_GameState);
    };

    addAndMakeVisible(Board3);
    Board3.setButtonText("8/8/8/8/8/8/8/8");
    Board3.onClick = [this](){
        m_GameState.setBoardFen("8/8/8/8/8/8/8/8");
        m_ThreatsSonifier.onMove(m_GameState);
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
    m_ThreatsSonifier.prepareToPlay(samplesPerBlockExpected,sampleRate);
    m_ThreatsSonifier.onMove(m_GameState);
}


void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    m_ThreatsSonifier.process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.buffer->getNumSamples());
}

void MainComponent::releaseResources()
{
    m_ThreatsSonifier.releaseResources();
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

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(600, 400);
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


    addAndMakeVisible(m_ChessboardGUI);
    Chess::Game& game = AppState::getInstance().getGame();
    m_ChessboardGUI.addActionListener(&m_BroadcastManager);
    m_BroadcastManager.addChangeListener(&m_ChessboardGUI);
    m_BroadcastManager.addChangeListener(&m_DebugSonifier);

    // text buttons
    addAndMakeVisible(buttonPreset1);
    buttonPreset1.setButtonText("8/8/8/4p1K1/2k1P3/8/8/8");
    buttonPreset1.onClick = [this, &game]() {
        game.setBoardFen("8/8/8/4p1K1/2k1P3/8/8/8");
        m_BroadcastManager.sendChangeMessage();
    };

    addAndMakeVisible(buttonPreset2);
    buttonPreset2.setButtonText("4k2r/6r1/8/8/8/8/3R4/R3K3");
    buttonPreset2.onClick = [this, &game]() {
        game.setBoardFen("4k2r/6r1/8/8/8/8/3R4/R3K3");
        m_BroadcastManager.sendChangeMessage();
    };

    addAndMakeVisible(buttonPreset3);
    buttonPreset3.setButtonText("8/8/8/8/8/8/8/8");
    buttonPreset3.onClick = [this, &game]() {
        game.setBoardFen("8/8/8/8/8/8/8/8");
        m_BroadcastManager.sendChangeMessage();
    };

    addAndMakeVisible(m_SonifierSelector);
    m_SonifierSelector.onChange = [this]() { onSonifierChange(); };
    m_SonifierSelector.addItem("Debug Sonifier", 1);
    m_SonifierSelector.addItem("Threat Sonifier", 2);
    m_SonifierSelector.setSelectedId(1);
}

MainComponent::~MainComponent()
{
    m_BroadcastManager.removeAllChangeListeners();
    m_ChessboardGUI.removeAllActionListeners();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_DebugSonifier.prepareToPlay(samplesPerBlockExpected, sampleRate);
    // m_ThreatSonifier.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    m_DebugSonifier.process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    // m_CurrentSonifier.process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    m_DebugSonifier.releaseResources();
    // m_ThreatSonifier.releaseResources():
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    auto area = getBounds().reduced(10);
    auto bottomLayer = area.removeFromBottom(getHeight() / 6);
    auto rightThird = area.removeFromRight(getWidth() / 3);
    rightThird.reduce(10, 10);

    m_ChessboardGUI.setBounds(area);
    m_SonifierSelector.setBounds(rightThird.removeFromBottom(getHeight() / 4).reduced(0, 20));

    buttonPreset1.setBounds(bottomLayer.removeFromLeft(getWidth() / 3));
    buttonPreset2.setBounds(bottomLayer.removeFromLeft(getWidth() / 3));
    buttonPreset3.setBounds(bottomLayer.removeFromLeft(getWidth() / 3));
}

void MainComponent::onSonifierChange()
{
    switch (m_SonifierSelector.getSelectedId())
    {
    case 1:
        //m_CurrentSonifier = &m_DebugSonifier;
        break;
    default:
        //m_CurrentSonifier = &m_ThreatSonifier;
        ;
    }

}

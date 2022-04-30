#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(1000, 800);

    setAudioChannels(0, 2);

    addAndMakeVisible(m_ChessboardGUI);
    Chess::Game& game = AppState::getInstance().getGame();
    m_ChessboardGUI.addActionListener(&m_BroadcastManager);
    m_BroadcastManager.addChangeListener(&m_ChessboardGUI);
    m_BroadcastManager.addChangeListener(this);

    addAndMakeVisible(buttonUndo);
    buttonUndo.setButtonText("Undo");
    buttonUndo.onClick = [this, &game]() {
        m_BroadcastManager.undo();
        if (m_GameMode == GameMode::PVC)
            m_BroadcastManager.undo();
    };

    addAndMakeVisible(buttonRedo);
    buttonRedo.setButtonText("Redo");
    buttonRedo.onClick = [this, &game]() {
        m_BroadcastManager.redo();
        if (m_GameMode == GameMode::PVC)
            m_BroadcastManager.redo();
    };

    addAndMakeVisible(m_SonifierSelector);
    m_SonifierSelector.onChange = [this]() 
    {
        setSonifier(m_SonifierSelector.getSelectedItemIndex());
    };
    for (int i = 0; i < sonifiers.size(); i++) {
        m_SonifierSelector.addItem(sonifiers[i].name + " Sonifier", i + 1);
    }
    m_SonifierSelector.setSelectedId(1, juce::dontSendNotification);
    mCurrentSonifier->setEnabled(true);

    addAndMakeVisible(m_GameModeSelector);
    m_GameModeSelector.onChange = [this]()
    {
        switch (m_GameModeSelector.getSelectedId())
        {
        case 1: onGameModeChange(GameMode::PVP); break;
        case 2: onGameModeChange(GameMode::PVC); break;
        default: onGameModeChange(GameMode::PGN);
        }
    };
    m_GameModeSelector.addItem("PVP", 1);
    m_GameModeSelector.addItem("PVC", 2);
    m_GameModeSelector.addItem("PGN", 3);
    m_GameModeSelector.setSelectedId(1);

    addAndMakeVisible(m_TurnText);
    m_TurnText.setText("White's turn", juce::NotificationType::dontSendNotification);
    m_TurnText.setFont(juce::Font(15));
    m_TurnText.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    m_TurnText.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    m_TurnText.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(m_pgnButton);
    m_pgnButton.setButtonText("Load PGN");
    m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
    m_pgnButton.onClick = [this]() { onPgnButtonClicked(); };

    addAndMakeVisible(m_PrevButton);
    m_PrevButton.setButtonText("Previous");

    addAndMakeVisible(m_NextButton);
    m_NextButton.setButtonText("Next");

    addAndMakeVisible(m_VolumeSlider);
    m_VolumeSlider.setRange(0, 0.25);
    m_VolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    m_VolumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    m_VolumeSlider.onValueChange = [this]() {
        mCurrentSonifier->setGain(m_VolumeSlider.getValue());
    };
    m_VolumeSlider.setValue(0.25);
}

MainComponent::~MainComponent()
{
    m_BroadcastManager.removeAllChangeListeners();
    m_BroadcastManager.removeAllActionListeners();
    m_ChessboardGUI.removeAllActionListeners();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    this->samplesPerBlockExpected = samplesPerBlockExpected;
    this->sampleRate = sampleRate;
    setSonifier(0);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (mOldSonifier) {
        mOldSonifier->process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        if (mOldSonifier->isIdle()) {
            // TODO: Maybe not in the audio thread.
            mOldSonifier.reset();
        }
    }
    mCurrentSonifier->process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    auto area = getBounds().reduced(10);
    auto footer = area.removeFromBottom(getHeight() / 10);
    auto rightThird = area.removeFromRight(getWidth() / 3);
    rightThird.reduce(10, 10);

    auto sliderArea = rightThird.removeFromRight(rightThird.getWidth() / 6);
    m_VolumeSlider.setBounds(sliderArea);

    m_TurnText.setBounds(area.removeFromBottom(area.getHeight() / 15).reduced(0, 5));
    
    auto areaAboveChessboard = area.removeFromTop(area.getHeight() / 12);
    buttonUndo.setBounds(areaAboveChessboard.removeFromLeft(areaAboveChessboard.getWidth() / 2));
    buttonRedo.setBounds(areaAboveChessboard);
    // Keep chessboard square and centered.
    int size = std::min(area.getWidth(), area.getHeight());
    m_ChessboardGUI.setBounds(area.withSizeKeepingCentre(size, size));

    auto rightBottomThird = rightThird.removeFromBottom(rightThird.getHeight() / 3).reduced(20);
    m_SonifierSelector.setBounds(rightBottomThird.removeFromLeft(rightBottomThird.getWidth() / 2));
    m_GameModeSelector.setBounds(rightBottomThird);
    m_pgnButton.setBounds(rightThird.removeFromTop(rightThird.getHeight() / 2).reduced(20));

    m_PrevButton.setBounds(rightThird.removeFromLeft(rightThird.getWidth() / 2).reduced(20));
    m_NextButton.setBounds(rightThird.reduced(20));
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    Chess::Game& game = AppState::getInstance().getGame();
    switch (game.getTurn())
    {
    case Chess::Color::White:
        m_TurnText.setText("White's Turn", juce::dontSendNotification);
        break;
    default:
        m_TurnText.setText("Black's Turn", juce::dontSendNotification);
    }
}

void MainComponent::setSonifier(int sonifierIndex)
{
    mOldSonifier = std::move(mCurrentSonifier);
    mCurrentSonifier = sonifiers[sonifierIndex].create();
    if (mOldSonifier) {
        mOldSonifier->setEnabled(false);
        m_BroadcastManager.removeChangeListener(mOldSonifier.get());
        m_BroadcastManager.removeActionListener(mOldSonifier.get());
    }
    mCurrentSonifier->prepareToPlay(samplesPerBlockExpected, sampleRate);
    mCurrentSonifier->setEnabled(true);
    m_BroadcastManager.addChangeListener(mCurrentSonifier.get());
    m_BroadcastManager.addActionListener(mCurrentSonifier.get());
    mCurrentSonifier->onMove(AppState::getInstance().getGame());
    mCurrentSonifier->setGain(m_VolumeSlider.getValue());
}

void MainComponent::onPgnButtonClicked()
{
    m_FileChooser = std::make_unique<juce::FileChooser>("Please select the .pgn file you want to load...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.pgn");

    auto folderChooserFlags = juce::FileBrowserComponent::openMode;

    m_FileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser)
        {
            juce::File file = chooser.getResult();
            if (file.exists())
            {
                m_pgnButton.setButtonText("PGN Loaded!");
                m_pgnButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
                m_PgnString = chooser.getResult().loadFileAsString();
                m_NextButton.setEnabled(true);
                m_PrevButton.setEnabled(true);
            }
            else {
                if (m_PgnString.isEmpty())
                {
                    m_pgnButton.setButtonText("Load PGN");
                    m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
                    m_NextButton.setEnabled(false);
                    m_PrevButton.setEnabled(false);
                }
            }
        });


}

void MainComponent::onGameModeChange(MainComponent::GameMode nextGameMode)
{
    switch (nextGameMode)
    {
    case GameMode::PVC:
        m_BroadcastManager.toggleStockfish(true);
        m_ChessboardGUI.setMode(BoardComponent::Mode::PVC);
        m_pgnButton.setButtonText("Load PGN");
        m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
        m_PgnString.clear();
        m_pgnButton.setEnabled(false);
        m_PrevButton.setEnabled(false);
        m_NextButton.setEnabled(false);
        buttonUndo.setEnabled(true);
        buttonRedo.setEnabled(true);
        break;
    case GameMode::PVP:
        m_BroadcastManager.toggleStockfish(false);
        m_ChessboardGUI.setMode(BoardComponent::Mode::PVP);
        m_pgnButton.setButtonText("Load PGN");
        m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
        m_PgnString.clear();
        m_pgnButton.setEnabled(false);
        m_PrevButton.setEnabled(false);
        m_NextButton.setEnabled(false);
        buttonUndo.setEnabled(true);
        buttonRedo.setEnabled(true);
        break;
    default:
        m_BroadcastManager.toggleStockfish(false);
        m_ChessboardGUI.setMode(BoardComponent::Mode::PGN);
        m_pgnButton.setEnabled(true);
        buttonUndo.setEnabled(false);
        buttonRedo.setEnabled(false);
    }
    m_GameMode = nextGameMode;
    m_BroadcastManager.emptyUndoHistory();
    AppState::getInstance().getGame().setFen(AppState::getInstance().getGame().initialFen);
    m_BroadcastManager.sendChangeMessage();
}

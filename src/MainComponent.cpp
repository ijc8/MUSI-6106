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

    addAndMakeVisible(buttonPreset4);
    buttonPreset4.setButtonText("rnbqkb1r/p1pp2pp/3n4/8/P3N3/8/1P1pppPP/R1BQKBNR");
    buttonPreset4.onClick = [this, &game]() {
        game.setBoardFen("rnbqkb1r/p1pp2pp/3n4/8/P3N3/8/1P1pppPP/R1BQKBNR");
        m_BroadcastManager.sendChangeMessage();
    };

    addAndMakeVisible(buttonPreset5);
    buttonPreset5.setButtonText("3r1rk1/ppp1qpp1/3p1n1p/2b1p3/2BnP3/2NP1N2/PPP1QPPP/R1B2RK1");
    buttonPreset5.onClick = [this, &game]() {
        game.setBoardFen("3r1rk1/ppp1qpp1/3p1n1p/2b1p3/2BnP3/2NP1N2/PPP1QPPP/R1B2RK1");
        m_BroadcastManager.sendChangeMessage();
    };

    addAndMakeVisible(buttonReset);
    buttonReset.setButtonText("Reset Game");
    buttonReset.onClick = [this, &game]() {
        game.setFen(AppState::getInstance().getGame().initialFen);
        m_BroadcastManager.sendChangeMessage();
    };

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

    addAndMakeVisible(m_TitleText);
    m_TitleText.setText("MUSICAL CHESS", juce::NotificationType::dontSendNotification);
    m_TitleText.setFont(juce::Font(30));
    m_TitleText.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    m_TitleText.setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    m_TitleText.setJustificationType(juce::Justification::centred);

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
    auto header = area.removeFromTop(getHeight() / 10);
    auto footer = area.removeFromBottom(getHeight() / 6);
    auto rightThird = area.removeFromRight(getWidth() / 3);
    rightThird.reduce(10, 10);

    auto sliderArea = rightThird.removeFromRight(rightThird.getWidth() / 6);
    m_VolumeSlider.setBounds(sliderArea);

    m_TitleText.setBounds(header);

    m_TurnText.setBounds(area.removeFromBottom(area.getHeight() / 15).reduced(0, 5));
    
    auto areaAboveChessboard = area.removeFromTop(area.getHeight() / 12);
    buttonUndo.setBounds(areaAboveChessboard.removeFromLeft(areaAboveChessboard.getWidth() / 2));
    buttonRedo.setBounds(areaAboveChessboard);
    m_ChessboardGUI.setBounds(area);

    auto rightBottomThird = rightThird.removeFromBottom(rightThird.getHeight() / 3).reduced(20);
    m_SonifierSelector.setBounds(rightBottomThird.removeFromLeft(rightBottomThird.getWidth() / 2));
    m_GameModeSelector.setBounds(rightBottomThird);
    m_pgnButton.setBounds(rightThird.removeFromTop(rightThird.getHeight() / 2).reduced(20));

    m_PrevButton.setBounds(rightThird.removeFromLeft(rightThird.getWidth() / 2).reduced(20));
    m_NextButton.setBounds(rightThird.reduced(20));

    buttonPreset1.setBounds(footer.removeFromLeft(getWidth() / 6));
    buttonPreset2.setBounds(footer.removeFromLeft(getWidth() / 6));
    buttonPreset3.setBounds(footer.removeFromLeft(getWidth() / 6));
    buttonPreset4.setBounds(footer.removeFromLeft(getWidth() / 6));
    buttonPreset5.setBounds(footer.removeFromLeft(getWidth() / 6));
    buttonReset.setBounds(footer.removeFromLeft(getWidth() / 6));
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
        m_ChessboardGUI.onModeChange(GUI::ChessBoard::mode::kPVC);
        m_pgnButton.setButtonText("Load PGN");
        m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
        m_PgnString.clear();
        m_pgnButton.setEnabled(false);
        m_PrevButton.setEnabled(false);
        m_NextButton.setEnabled(false);
        buttonPreset1.setEnabled(true);
        buttonPreset2.setEnabled(true);
        buttonPreset3.setEnabled(true);
        buttonPreset4.setEnabled(true);
        buttonPreset5.setEnabled(true);
        buttonReset.setEnabled(true);
        buttonUndo.setEnabled(true);
        buttonRedo.setEnabled(true);
        break;
    case GameMode::PVP:
        m_BroadcastManager.toggleStockfish(false);
        m_ChessboardGUI.onModeChange(GUI::ChessBoard::mode::kPVP);
        m_pgnButton.setButtonText("Load PGN");
        m_pgnButton.setColour(juce::TextButton::buttonColourId, getLookAndFeel().findColour(juce::TextButton::buttonColourId));
        m_PgnString.clear();
        m_pgnButton.setEnabled(false);
        m_PrevButton.setEnabled(false);
        m_NextButton.setEnabled(false);
        buttonPreset1.setEnabled(true);
        buttonPreset2.setEnabled(true);
        buttonPreset3.setEnabled(true);
        buttonPreset4.setEnabled(true);
        buttonPreset5.setEnabled(true);
        buttonReset.setEnabled(true);
        buttonUndo.setEnabled(true);
        buttonRedo.setEnabled(true);
        break;
    default:
        m_BroadcastManager.toggleStockfish(false);
        m_ChessboardGUI.onModeChange(GUI::ChessBoard::mode::kPGN);
        m_pgnButton.setEnabled(true);
        buttonPreset1.setEnabled(false);
        buttonPreset2.setEnabled(false);
        buttonPreset3.setEnabled(false);
        buttonPreset4.setEnabled(false);
        buttonPreset5.setEnabled(false);
        buttonReset.setEnabled(false);
        buttonUndo.setEnabled(false);
        buttonRedo.setEnabled(false);
    }
    m_GameMode = nextGameMode;
    m_BroadcastManager.emptyUndoHistory();
    AppState::getInstance().getGame().setFen(AppState::getInstance().getGame().initialFen);
    m_BroadcastManager.sendChangeMessage();
}

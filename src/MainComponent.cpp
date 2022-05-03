#include "MainComponent.h"

MainComponent::MainComponent() {
    setSize(1000, 740);

    setAudioChannels(0, 2);

    addAndMakeVisible(board);
    Chess::Game &game = AppState::getInstance().getGame();
    board.onMove = [this](Chess::Move move) { broadcastManager.makeMove(move); };
    broadcastManager.addChangeListener(&board);
    broadcastManager.addChangeListener(this);

    // prevButton.setButtonText("Undo");
    // prevButton.onClick = [this, &game]() {
    //     broadcastManager.undo();
    //     if (mode == GameMode::PVC)
    //         broadcastManager.undo();
    // };

    // nextButton.setButtonText("Redo");
    // nextButton.onClick = [this, &game]() {
    //     broadcastManager.redo();
    //     if (mode == GameMode::PVC)
    //         broadcastManager.redo();
    // };

    // addAndMakeVisible(sonifierMenu);
    // sonifierMenu.onChange = [this]() {
    //     setSonifier(sonifierMenu.getSelectedItemIndex());
    // };
    // for (int i = 0; i < sonifiers.size(); i++) {
    //     sonifierMenu.addItem(sonifiers[i].name, i + 1);
    // }
    // sonifierMenu.setSelectedId(1, juce::dontSendNotification);
    // currentSonifier->setEnabled(true);

    // addAndMakeVisible(sonifierLabel);
    // sonifierLabel.setText("Sonifier", juce::dontSendNotification);
    // sonifierLabel.attachToComponent(&sonifierMenu, false);

    // TODO
    // modeMenu.onChange = [this]() {
    //     switch (modeMenu.getSelectedId()) {
    //     case 1:
    //         onGameModeChange(GameMode::PVP);
    //         break;
    //     case 2:
    //         onGameModeChange(GameMode::PVC);
    //         break;
    //     default:
    //         onGameModeChange(GameMode::PGN);
    //     }
    // };

    // TODO
    // fenInput.onTextChange = [this]() { onFenChanged(); };

    addAndMakeVisible(turnLabel);
    turnLabel.setFont(juce::Font(15));
    turnLabel.setJustificationType(juce::Justification::centred);
    changeListenerCallback(nullptr);

    // TODO
    // openPGN.onClick = [this]() { loadSavedGame(); };

    // TODO
    // volumeSlider.setRange(0, 0.25);
    // volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    // volumeSlider.onValueChange = [this]() {
    //     // TODO: Logarithmic scaling! (i.e. set gain in dB)
    //     currentSonifier->setGain(volumeSlider.getValue());
    // };
    // volumeSlider.setValue(0.25);

    // TODO
    // streamInputLabel.setText("Lichess Game ID", juce::dontSendNotification);
    // streamInputLabel.attachToComponent(&streamInput, false);
    // streamToggle.setButtonText("Play Stream");
    // streamToggle.onClick = [this]() {
    //     if (stream) {
    //         // Hack to avoid thread-killing issues.
    //         stream->cancel();
    //         streams.push_back(stream);
    //         stream.reset();
    //         streamToggle.setButtonText("Play Stream");
    //     } else {
    //         // TODO: Reset game first.
    //         std::string id = streamInput.getText().toStdString();
    //         stream = std::make_unique<GameStream>(id, [this](std::optional<Chess::Move> move) {
    //             if (move) {
    //                 std::cout << "Streamed move: " << move->toString() << std::endl;
    //                 broadcastManager.actionListenerCallback(juce::String(move->toString()));
    //             } else {
    //                 std::cout << "Done streaming." << std::endl;
    //             }
    //         });
    //         streamToggle.setButtonText("Stop Stream");
    //     }
    // };

    playerOptions.blackMenu.onChange = [this]() {
        int id = playerOptions.blackMenu.getSelectedId();
        board.enableInput(Chess::Color::Black, id == 1);
    };

    playerOptions.whiteMenu.onChange = [this]() {
        int id = playerOptions.whiteMenu.getSelectedId();
        board.enableInput(Chess::Color::White, id == 1);
    };

    addAndMakeVisible(controls);
    addAndMakeVisible(playerOptions);
    addAndMakeVisible(soundOptions);
    addAndMakeVisible(analysisOptions);
}

MainComponent::~MainComponent() {
    broadcastManager.removeAllChangeListeners();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    (void)samplesPerBlockExpected;
    this->sampleRate = sampleRate;
    setSonifier(0);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (oldSonifier) {
        oldSonifier->process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        if (oldSonifier->isIdle()) {
            // TODO: Maybe not in the audio thread.
            oldSonifier.reset();
        }
    }
    if (currentSonifier) {
        currentSonifier->process(bufferToFill.buffer->getArrayOfWritePointers(), bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    }
}

void MainComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    auto area = getBounds().reduced(10);

    // Keep chessboard square and centered.
    int menuWidth = 300;
    int turnHeight = 30;
    int size = std::min(area.getWidth() - menuWidth, area.getHeight() - turnHeight);
    area = area.withSizeKeepingCentre(size + menuWidth, size + turnHeight);
    auto menuArea = area.removeFromRight(menuWidth);
    turnLabel.setBounds(area.removeFromBottom(turnHeight));
    board.setBounds(area);

    // Lay out menu sections.
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    juce::FlexItem::Margin margin(24, 6, 6, 6);
    fb.items.add(juce::FlexItem(controls).withMinHeight(170).withMargin(6));
    fb.items.add(juce::FlexItem(playerOptions).withMinHeight(150).withMargin(margin));
    fb.items.add(juce::FlexItem(analysisOptions).withMinHeight(170).withMargin(margin));
    fb.items.add(juce::FlexItem(soundOptions).withMinHeight(140).withMargin(margin));
    fb.performLayout(menuArea);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    Chess::Game &game = AppState::getInstance().getGame();
    switch (game.getTurn()) {
    case Chess::Color::White:
        turnLabel.setText("White to move", juce::dontSendNotification);
        turnLabel.setColour(turnLabel.backgroundColourId, juce::Colours::whitesmoke);
        turnLabel.setColour(turnLabel.textColourId, juce::Colours::black);
        break;
    default:
        turnLabel.setText("Black to move", juce::dontSendNotification);
        turnLabel.setColour(turnLabel.backgroundColourId, juce::Colours::black);
        turnLabel.setColour(turnLabel.textColourId, juce::Colours::whitesmoke);
    }
}

void MainComponent::setSonifier(int sonifierIndex) {
    oldSonifier = std::move(currentSonifier);
    currentSonifier = sonifiers[sonifierIndex].create(sampleRate);
    if (oldSonifier) {
        oldSonifier->setEnabled(false);
        broadcastManager.removeChangeListener(oldSonifier.get());
    }
    currentSonifier->setEnabled(true);
    broadcastManager.addChangeListener(currentSonifier.get());
    currentSonifier->onMove(AppState::getInstance().getGame());
    // currentSonifier->setGain(volumeSlider.getValue());
}

void MainComponent::setFEN() {
    // TODO
    // std::string fenString = fenInput.getText().toStdString();
    // AppState::getInstance().getGame().setFen(fenString);
    // broadcastManager.sendChangeMessage();
}

void MainComponent::loadSavedGame() {
    fileChooser = std::make_unique<juce::FileChooser>("Please select the .pgn file you want to load...",
                                                      juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                      "*.pgn");

    auto folderChooserFlags = juce::FileBrowserComponent::openMode;

    fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser &chooser) {
        juce::File file = chooser.getResult();
        if (file.exists()) {
            // pgnData = chooser.getResult().loadFileAsString();
        }
    });
}

// void MainComponent::onGameModeChange(MainComponent::GameMode nextGameMode) {
//     switch (nextGameMode) {
//     case GameMode::PVC:
//         broadcastManager.toggleStockfish(true);
//         board.setMode(BoardComponent::Mode::PVC);
//         pgnData.clear();
//         // openPGN.setEnabled(false);
//         break;
//     case GameMode::PVP:
//         broadcastManager.toggleStockfish(false);
//         board.setMode(BoardComponent::Mode::PVP);
//         pgnData.clear();
//         break;
//     default:
//         broadcastManager.toggleStockfish(false);
//         board.setMode(BoardComponent::Mode::PGN);
//     }
//     mode = nextGameMode;
//     broadcastManager.clearRedoStack();
//     AppState::getInstance().getGame().setFen(AppState::getInstance().getGame().initialFen);
//     broadcastManager.sendChangeMessage();
// }

#include "MainComponent.h"

Controls::Controls() {
    setText("Controls");
    setColour(ColourIds::textColourId, juce::Colours::lightgrey);

    addAndMakeVisible(move);
    move.setText("25/26", juce::dontSendNotification);
    move.setJustificationType(juce::Justification::centred);

    std::initializer_list<std::pair<const char *, juce::ImageButton *>> pairs = {
        {"fastbackwardsolid_png", &skipBackward},
        {"stepbackwardsolid_png", &stepBackward},
        {"stepforwardsolid_png", &stepForward},
        {"fastforwardsolid_png", &skipForward},
    };
    for (auto [name, button] : pairs) {
        int size;
        const char *data = ChessImageData::getNamedResource(name, size);
        juce::Image image = juce::ImageFileFormat::loadFrom(data, size);
        button->setImages(
            false, false, true,
            image, 0.6, juce::Colours::transparentBlack,
            image, 0.8, juce::Colours::transparentBlack,
            image, 1.0, juce::Colours::transparentBlack);
        addAndMakeVisible(*button);
    }

    juce::Image playImage = juce::ImageFileFormat::loadFrom(ChessImageData::playsolid_png, ChessImageData::playsolid_pngSize);
    juce::Image pauseImage = juce::ImageFileFormat::loadFrom(ChessImageData::pausesolid_png, ChessImageData::pausesolid_pngSize);
    playPause.setImages(
        false, false, true,
        playImage, 0.6, juce::Colours::transparentBlack,
        playImage, 0.8, juce::Colours::transparentBlack,
        pauseImage, 1.0, juce::Colours::transparentBlack);
    addAndMakeVisible(playPause);

    addAndMakeVisible(autoAdvance);
    autoAdvancePeriod.setJustification(juce::Justification::centredRight);
    autoAdvancePeriod.setText("5");
    autoAdvancePeriod.setInputRestrictions(0, "0123456789");
    addAndMakeVisible(autoAdvancePeriod);
    seconds.setText("secs", juce::dontSendNotification);
    addAndMakeVisible(seconds);

    addAndMakeVisible(pgnAdvance);

    resized();
}

void Controls::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add(juce::FlexItem(move).withMinHeight(16).withMargin(juce::FlexItem::Margin(18, 6, 0, 6)));

    juce::FlexBox buttons;
    buttons.items.add(juce::FlexItem(skipBackward).withFlex(1));
    buttons.items.add(juce::FlexItem(stepBackward).withFlex(1));
    buttons.items.add(juce::FlexItem(playPause).withFlex(1));
    buttons.items.add(juce::FlexItem(stepForward).withFlex(1));
    buttons.items.add(juce::FlexItem(skipForward).withFlex(1));

    fb.items.add(juce::FlexItem(buttons).withMinHeight(50).withMargin(juce::FlexItem::Margin(0, 6, 6, 6)));

    juce::FlexBox autoBox;
    autoAdvance.changeWidthToFitText();
    autoBox.items.add(juce::FlexItem(autoAdvance).withMinWidth(autoAdvance.getWidth()));
    autoBox.items.add(juce::FlexItem(autoAdvancePeriod).withMinWidth(30).withMaxHeight(25));
    autoBox.items.add(juce::FlexItem(seconds).withMinWidth(50));

    fb.items.add(juce::FlexItem(autoBox).withMinHeight(25).withMargin(juce::FlexItem::Margin(6, 12, 6, 12)));
    fb.items.add(juce::FlexItem(pgnAdvance).withMinHeight(20).withMargin(juce::FlexItem::Margin(6, 12, 6, 12)));
    fb.performLayout(getLocalBounds());
}

PlayerOptions::PlayerOptions() {
    setText("Players");
    setColour(ColourIds::textColourId, juce::Colours::lightgrey);

    whiteLabel.setText("White", juce::dontSendNotification);
    blackLabel.setText("Black", juce::dontSendNotification);
    addAndMakeVisible(whiteLabel);
    whiteLabel.attachToComponent(&whiteMenu, false);
    addAndMakeVisible(blackLabel);
    blackLabel.attachToComponent(&blackMenu, false);

    for (auto menu : {&whiteMenu, &blackMenu}) {
        addAndMakeVisible(*menu);
        menu->addItem("Human", 1);
        menu->addItem("Computer (Easy)", 2);
        menu->addItem("Computer (Medium)", 3);
        menu->addItem("Computer (Hard)", 4);
        menu->setSelectedId(1, juce::dontSendNotification);
    }
}

void PlayerOptions::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignContent = juce::FlexBox::AlignContent::center;
    // Order matches board orientation.
    fb.items.add(juce::FlexItem(blackMenu).withMinHeight(30).withMargin(juce::FlexItem::Margin(40, 12, 6, 12)));
    fb.items.add(juce::FlexItem(whiteMenu).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
    fb.performLayout(getLocalBounds());
}

SoundOptions::SoundOptions() {
    setText("Sound");
    setColour(ColourIds::textColourId, juce::Colours::lightgrey);

    sonifierLabel.setText("Sonifier", juce::dontSendNotification);
    volumeLabel.setText("Volume", juce::dontSendNotification);
    addAndMakeVisible(volumeLabel);
    sonifierLabel.attachToComponent(&sonifierMenu, false);
    addAndMakeVisible(sonifierLabel);
    volumeLabel.attachToComponent(&volumeSlider, false);

    addAndMakeVisible(sonifierMenu);
    sonifierMenu.addItem("Zen", 1);
    sonifierMenu.addItem("Explosions", 2);
    sonifierMenu.setSelectedId(1);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
}

AnalysisOptions::AnalysisOptions() {
    setText("Analysis");
    setColour(ColourIds::textColourId, juce::Colours::lightgrey);

    loadGame.setButtonText("Load saved game (PGN)");
    streamGame.setButtonText("Stream live game (Lichess)");
    fenLabel.setText("FEN", juce::dontSendNotification);
    fenLabel.attachToComponent(&fen, false);
    fen.setText(Chess::Game::initialFen, false);

    addAndMakeVisible(loadGame);
    addAndMakeVisible(streamGame);
    addAndMakeVisible(fenLabel);
    addAndMakeVisible(fen);
}

void AnalysisOptions::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add(juce::FlexItem(loadGame).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
    fb.items.add(juce::FlexItem(streamGame).withMinHeight(30).withMargin(juce::FlexItem::Margin(6, 12, 6, 12)));
    fb.items.add(juce::FlexItem(fen).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
    fb.performLayout(getLocalBounds());
}

void SoundOptions::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add(juce::FlexItem(sonifierMenu).withMinHeight(30).withMargin(juce::FlexItem::Margin(40, 12, 6, 12)));
    fb.items.add(juce::FlexItem(volumeSlider).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
    fb.performLayout(getLocalBounds());
}

MainComponent::MainComponent() {
    setSize(1000, 740);

    setAudioChannels(0, 2);

    addAndMakeVisible(board);
    Chess::Game &game = AppState::getInstance().getGame();
    board.onMove = [this](Chess::Move move) { makeMove(move); };
    addChangeListener(&board);

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
    updateGame();

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
        players[(int)Chess::Color::Black] = (PlayerType)(id - 1);
        if (id > 1) {
            toggleStockfish(true);
        }
    };

    // TODO: Reduce duplication here.
    playerOptions.whiteMenu.onChange = [this]() {
        int id = playerOptions.whiteMenu.getSelectedId();
        board.enableInput(Chess::Color::White, id == 1);
        players[(int)Chess::Color::White] = (PlayerType)(id - 1);
        if (id > 1) {
            toggleStockfish(true);
        }
    };

    addAndMakeVisible(controls);
    addAndMakeVisible(playerOptions);
    addAndMakeVisible(soundOptions);
    addAndMakeVisible(analysisOptions);
}

MainComponent::~MainComponent() {
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

void MainComponent::updateGame() {
    Chess::Game &game = AppState::getInstance().getGame();
    if (game.getTurn() == Chess::Color::White) {
        turnLabel.setText("White to move", juce::dontSendNotification);
        turnLabel.setColour(turnLabel.backgroundColourId, juce::Colours::whitesmoke);
        turnLabel.setColour(turnLabel.textColourId, juce::Colours::black);

        if (players[(int)Chess::Color::White] != PlayerType::Human) {
            engineManager->generateMove();
        }
    } else {
        turnLabel.setText("Black to move", juce::dontSendNotification);
        turnLabel.setColour(turnLabel.backgroundColourId, juce::Colours::black);
        turnLabel.setColour(turnLabel.textColourId, juce::Colours::whitesmoke);

        if (players[(int)Chess::Color::Black] != PlayerType::Human) {
            engineManager->generateMove();
        }
    }

    sendChangeMessage();
}

void MainComponent::setSonifier(int sonifierIndex) {
    oldSonifier = std::move(currentSonifier);
    currentSonifier = sonifiers[sonifierIndex].create(sampleRate);
    if (oldSonifier) {
        oldSonifier->setEnabled(false);
        removeChangeListener(oldSonifier.get());
    }
    currentSonifier->setEnabled(true);
    addChangeListener(currentSonifier.get());
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

void MainComponent::makeMove(Chess::Move move) {
    clearRedoStack();
    game.push(move);
    updateGame();
}

void MainComponent::undo() {
    std::optional<Chess::Move> move = game.pop();
    if (move) {
        redoStack.push(*move);
        updateGame();
    }
}

void MainComponent::redo() {
    if (!redoStack.empty()) {
        Chess::Move lastMove = redoStack.top();
        game.push(lastMove);
        redoStack.pop();
        updateGame();
    }
}

void MainComponent::clearRedoStack() {
    std::stack<Chess::Move> empty;
    redoStack.swap(empty);
}

void MainComponent::toggleStockfish(bool shouldTurnOn) {
    if (shouldTurnOn) {
        if (engineManager) {
            // Already started.
            updateGame();
        } else if (std::filesystem::exists("../../stockfish/stockfish_14.1_win_x64_avx2.exe")) {
            engineManager = std::make_unique<EngineManager>("../../stockfish/stockfish_14.1_win_x64_avx2.exe");
            // engineManager->onMove = [this](Chess::Move move) { makeMove(move); };
            updateGame();
        } else {
            // Allow user to tell us where their engine binary is.
            // TODO: Remember their selected engine and allow them to change it later.
            engineChooser = std::make_unique<juce::FileChooser>("Please select the engine executable you want to use...",
                                                                juce::File::getSpecialLocation(juce::File::userHomeDirectory));

            auto chooserFlags = juce::FileBrowserComponent::openMode;

            engineChooser->launchAsync(chooserFlags, [this](const juce::FileChooser &chooser) {
                juce::File file = chooser.getResult();
                if (file.exists()) {
                    engineManager = std::make_unique<EngineManager>(file.getFullPathName().toStdString());
                    engineManager->onMove = [this](Chess::Move move) {
                        // juce::MessageManager::callAsync([this, move]() { makeMove(move); });
                    };
                    updateGame();
                } else {
                    // User canceled.
                    // TODO: Reset triggering player to "Human".
                }
            });
        }
    } else {
        engineManager.reset();
    }
}

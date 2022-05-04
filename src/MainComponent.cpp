#include <filesystem>

#include "MainComponent.h"

// Utilities for loading and saving user's engine preference.
std::string loadEnginePath() {
    juce::File path = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                          .getChildFile("MusicalChess")
                          .getChildFile("engine_path.txt");
    if (!path.exists()) return "";
    return path.loadFileAsString().toStdString();
}

void saveEnginePath(const std::string &enginePath) {
    juce::File path = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                          .getChildFile("MusicalChess")
                          .getChildFile("engine_path.txt");
    if (!path.exists()) {
        path.create();
        juce::FileOutputStream output(path);
        output.writeText(enginePath, false, false, "");
    }
}

Controls::Controls() {
    setText("Controls");
    setColour(ColourIds::textColourId, juce::Colours::lightgrey);

    addAndMakeVisible(move);
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
    // Ideally this would show a brighter pause icon when we hover over the button in the "toggle on" state
    // (the same way it shows a brighter play icon when we hover over it in the "toggle off" state),
    // but unfortunately this doesn't seem to be supported by JUCE.
    playPause.setImages(
        false, false, true,
        playImage, 0.6, juce::Colours::transparentBlack,
        playImage, 0.8, juce::Colours::transparentBlack,
        pauseImage, 0.6, juce::Colours::transparentBlack);
    addAndMakeVisible(playPause);

    addAndMakeVisible(autoAdvance);
    autoAdvancePeriod.setJustification(juce::Justification::centredRight);
    autoAdvancePeriod.setText("3");
    autoAdvancePeriod.setInputRestrictions(0, "0123456789.");
    addAndMakeVisible(autoAdvancePeriod);
    addAndMakeVisible(seconds);

    // addAndMakeVisible(pgnAdvance);

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
    float autoAdvanceWidth = autoAdvance.getFont().getStringWidthFloat(autoAdvance.getText());
    autoBox.items.add(juce::FlexItem(autoAdvance).withMinWidth(autoAdvanceWidth + 10));
    autoBox.items.add(juce::FlexItem(autoAdvancePeriod).withMinWidth(30).withMaxHeight(25));
    float secondsWidth = seconds.getFont().getStringWidthFloat(seconds.getText());
    autoBox.items.add(juce::FlexItem(seconds).withMinWidth(secondsWidth + 10));

    fb.items.add(juce::FlexItem(autoBox).withMinHeight(20).withMargin(juce::FlexItem::Margin(0, 12, 6, 12)));
    // fb.items.add(juce::FlexItem(pgnAdvance).withMinHeight(20).withMargin(juce::FlexItem::Margin(6, 12, 6, 12)));
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
    sonifierLabel.attachToComponent(&sonifierMenu, false);
    addAndMakeVisible(sonifierLabel);
    addAndMakeVisible(sonifierMenu);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeSlider, false);
    addAndMakeVisible(volumeLabel);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    volumeSlider.setRange(-60, 0);
    addAndMakeVisible(volumeSlider);
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

double SoundOptions::getGain() const {
    double db = volumeSlider.getValue();
    double min = volumeSlider.getMinimum();
    return juce::Decibels::decibelsToGain(db, min);
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

MainComponent::MainComponent() {
    setSize(1000, 740);

    setAudioChannels(0, 2);

    addAndMakeVisible(board);
    board.onMove = [this](Chess::Move move) { makeMove(move); };

    addAndMakeVisible(turnLabel);
    turnLabel.setFont(juce::Font(15));
    turnLabel.setJustificationType(juce::Justification::centred);

    // Controls
    // TODO: Would be nice to support PGN comment timing.
    controls.skipBackward.onClick = [this]() {
        while (undo());
        updateGame();
    };

    controls.stepBackward.onClick = [this]() {
        Chess::Color turn = game.getTurn();
        if (players[(int)turn] == PlayerType::Human && players[!(int)turn] != PlayerType::Human) {
            // Special case: human vs. computer - go back one full move to the human's last turn.
            undo();
        }
        undo();
        updateGame();
    };

    controls.stepForward.onClick = [this]() {
        if (redo()) updateGame();
    };

    controls.skipForward.onClick = [this]() {
        while (redo())
            ;
        updateGame();
    };

    controls.playPause.setToggleable(true);
    controls.playPause.setClickingTogglesState(true);
    controls.playPause.onClick = [this]() {
        bool enabled = controls.playPause.getToggleState();
        if (enabled) {
            double period;
            std::string text = controls.autoAdvancePeriod.getText().toStdString();
            try {
                period = std::stod(text);
            } catch (...) {
                // Conversion failed, bail.
                // (Could be even nicer to disable the play button if the period is invalid;
                // however, wouldn't want to disable the pause button if it's already playing.)
                controls.playPause.setToggleState(false, juce::dontSendNotification);
                return;
            }
            startTimer(period * 1000);
        } else {
            stopTimer();
        }
    };

    controls.autoAdvancePeriod.onReturnKey = [this]() {
        if (isTimerRunning()) {
            // Change timer period.
            double period;
            std::string text = controls.autoAdvancePeriod.getText().toStdString();
            try {
                period = std::stod(text);
            } catch (...) {
                return;
            }
            startTimer(period * 1000);
        }
    };

    addAndMakeVisible(controls);

    // Player options
    playerOptions.blackMenu.onChange = [this]() {
        setPlayerType(Chess::Color::Black, (PlayerType)(playerOptions.blackMenu.getSelectedId() - 1));
    };
    playerOptions.whiteMenu.onChange = [this]() {
        setPlayerType(Chess::Color::White, (PlayerType)(playerOptions.whiteMenu.getSelectedId() - 1));
    };
    addAndMakeVisible(playerOptions);

    analysisOptions.loadGame.onClick = [this]() { loadSavedGame(); };
    analysisOptions.streamGame.onClick = [this]() { streamLiveGame(); };
    analysisOptions.fen.onReturnKey = [this]() {
        // NOTE: We don't clear the undo history here;
        // it might be annoying for the user if we throw out
        // the whole game when they just want to experiment.
        std::stack<Chess::Move> empty;
        redoStack.swap(empty);
        game.setFen(analysisOptions.fen.getText().toStdString());
        updateGame();
    };
    addAndMakeVisible(analysisOptions);

    // Sound options
    soundOptions.sonifierMenu.onChange = [this]() {
        setSonifier(soundOptions.sonifierMenu.getSelectedItemIndex());
    };
    for (int i = 0; i < sonifiers.size(); i++) {
        soundOptions.sonifierMenu.addItem(sonifiers[i].name, i + 1);
    }
    soundOptions.sonifierMenu.setSelectedId(1, juce::dontSendNotification);
    currentSonifier->setEnabled(true);

    soundOptions.volumeSlider.onValueChange = [this]() {
        currentSonifier->setGain(soundOptions.getGain());
    };
    soundOptions.volumeSlider.setValue(-20);
    addAndMakeVisible(soundOptions);

    enginePath = loadEnginePath();
    updateGame();
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
    fb.items.add(juce::FlexItem(controls).withMinHeight(130).withMargin(6));
    fb.items.add(juce::FlexItem(playerOptions).withMinHeight(150).withMargin(margin));
    fb.items.add(juce::FlexItem(analysisOptions).withMinHeight(170).withMargin(margin));
    fb.items.add(juce::FlexItem(soundOptions).withMinHeight(140).withMargin(margin));
    fb.performLayout(menuArea);
}

void MainComponent::timerCallback() {
    if (redo()) {
        updateGame();
    } else {
        controls.playPause.setToggleState(false, juce::dontSendNotification);
        stopTimer();
    }
}

void MainComponent::setPlayerType(Chess::Color color, PlayerType type) {
    board.enableInput(color, type == PlayerType::Human);
    players[(int)color] = type;
    if (type != PlayerType::Human) {
        enableStockfish(true);
    } else if (players[!(int)color] == PlayerType::Human) {
        enableStockfish(false);
    }
}

void MainComponent::updateGame() {
    auto outcome = game.getOutcome();
    if (outcome) {
        if (*outcome) {
            std::string text = *outcome == Chess::Color::White ? "White" : "Black";
            turnLabel.setText(text + " wins by checkmate", juce::dontSendNotification);
            juce::Colour bg = *outcome == Chess::Color::White ? juce::Colours::whitesmoke : juce::Colours::black;
            juce::Colour fg = *outcome == Chess::Color::White ? juce::Colours::black : juce::Colours::whitesmoke;
            turnLabel.setColour(turnLabel.backgroundColourId, bg);
            turnLabel.setColour(turnLabel.textColourId, fg);
        } else {
            turnLabel.setText("Draw by stalemate", juce::dontSendNotification);
            turnLabel.setColour(turnLabel.backgroundColourId, juce::Colours::lightgrey);
            turnLabel.setColour(turnLabel.textColourId, juce::Colours::grey);
        }
    } else {
        Chess::Color turn = game.getTurn();
        std::string text = turn == Chess::Color::White ? "White" : "Black";
        turnLabel.setText(text + " to move", juce::dontSendNotification);
        juce::Colour bg = turn == Chess::Color::White ? juce::Colours::whitesmoke : juce::Colours::black;
        juce::Colour fg = turn == Chess::Color::White ? juce::Colours::black : juce::Colours::whitesmoke;
        turnLabel.setColour(turnLabel.backgroundColourId, bg);
        turnLabel.setColour(turnLabel.textColourId, fg);

        PlayerType player = players[(int)turn];
        if (player != PlayerType::Human) {
            static int depth[] = {5, 5, 13};
            static int skill[] = {-5, 7, 20};
            engine->analyzeAsync([this](Chess::Analysis analysis) {
                Chess::Move move = analysis.bestMove;
                juce::MessageManager::callAsync([this, move]() { makeMove(move); });
            }, game, depth[(int)player - 1], skill[(int)player - 1]);
        }
    }

    int pastMoves = game.getHistory().size();
    int totalMoves = pastMoves + redoStack.size();
    controls.skipBackward.setEnabled(pastMoves > 0);
    controls.stepBackward.setEnabled(pastMoves > 0);
    controls.stepForward.setEnabled(pastMoves < totalMoves);
    controls.skipForward.setEnabled(pastMoves < totalMoves);
    controls.playPause.setEnabled(pastMoves < totalMoves);
    if (pastMoves == totalMoves && controls.playPause.getToggleState()) {
        controls.playPause.setToggleState(false, juce::dontSendNotification);
        stopTimer();
    }
    controls.move.setText(std::to_string(pastMoves) + "/" + std::to_string(totalMoves), juce::dontSendNotification);
    analysisOptions.fen.setText(game.getFen(), false);
    board.select();
    currentSonifier->onMove(game);
}

void MainComponent::setSonifier(int sonifierIndex) {
    oldSonifier = std::move(currentSonifier);
    currentSonifier = sonifiers[sonifierIndex].create(sampleRate);
    if (oldSonifier) {
        oldSonifier->setEnabled(false);
    }
    currentSonifier->setEnabled(true);
    currentSonifier->onMove(AppState::getInstance().getGame());
    currentSonifier->setGain(soundOptions.getGain());
}

void MainComponent::loadSavedGame() {
    fileChooser = std::make_unique<juce::FileChooser>("Please select the .pgn file you want to load...",
                                                      juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                                                      "*.pgn");

    auto folderChooserFlags = juce::FileBrowserComponent::openMode;

    fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser &chooser) {
        juce::File file = chooser.getResult();
        if (file.exists()) {
            std::cout << "File contents:" << std::endl
                      << chooser.getResult().loadFileAsString() << std::endl;
        }
    });
}

void MainComponent::streamLiveGame() {
    if (stream) {
        // Hack to avoid thread-killing issues.
        stream->cancel();
        streams.push_back(stream);
        stream.reset();
        analysisOptions.streamGame.setButtonText("Stream live game (Lichess)");
        return;
    }

    streamPrompt = std::make_unique<juce::AlertWindow>(
        "Stream Live Game", "Enter Lichess game ID or username", juce::MessageBoxIconType::NoIcon);
    streamPrompt->addTextEditor("id", "", "Game ID");
    streamPrompt->addTextBlock("or");
    streamPrompt->addTextEditor("username", "", "Username");
    streamPrompt->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    streamPrompt->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));
    streamPrompt->enterModalState(true, juce::ModalCallbackFunction::create([this](int result) {
        std::string id = streamPrompt->getTextEditorContents("id").toStdString();
        std::string username = streamPrompt->getTextEditorContents("username").toStdString();
        std::cout << "Got: " << result << " " << id << " " << username << std::endl;
        if (!id.empty()) {
            // Reset game first.
            while (game.pop());
            clearRedoStack();
            // Stream moves from game.
            stream = std::make_unique<GameStream>(id, [this](std::optional<Chess::Move> move) {
                if (move) {
                    std::cout << "Streamed move: " << move->toString() << std::endl;
                    while (redo());
                    game.push(*move);
                    updateGame();
                } else {
                    std::cout << "Done streaming." << std::endl;
                    streamLiveGame();
                }
            });
            analysisOptions.streamGame.setButtonText("Stop streaming");
        }
        streamPrompt->exitModalState(result);
        streamPrompt->setVisible(false);
        streamPrompt.reset();
    }));
}

void MainComponent::makeMove(Chess::Move move) {
    clearRedoStack();
    game.push(move);
    updateGame();
}

bool MainComponent::undo() {
    std::optional<Chess::Move> move = game.pop();
    if (!move) return false;
    redoStack.push(*move);
    return true;
}

bool MainComponent::redo() {
    if (redoStack.empty()) return false;
    Chess::Move lastMove = redoStack.top();
    game.push(lastMove);
    redoStack.pop();
    return true;
}

void MainComponent::clearRedoStack() {
    std::stack<Chess::Move> empty;
    redoStack.swap(empty);
}

void MainComponent::enableStockfish(bool enable) {
    if (!enable) {
        engine.reset();
    } else if (engine) {
        // Already started.
        updateGame();
    } else if (!enginePath.empty() && std::filesystem::exists(enginePath)) {
        engine = std::make_unique<Chess::Engine>(enginePath);
        updateGame();
    } else {
        // Allow user to tell us where their engine binary is.
        engineChooser = std::make_unique<juce::FileChooser>("Please select the engine executable you want to use...",
                                                            juce::File::getSpecialLocation(juce::File::globalApplicationsDirectory));
        auto chooserFlags = juce::FileBrowserComponent::openMode;

        engineChooser->launchAsync(chooserFlags, [this](const juce::FileChooser &chooser) {
            juce::File file = chooser.getResult();
            if (file.exists()) {
                enginePath = file.getFullPathName().toStdString();
                saveEnginePath(enginePath);
                engine = std::make_unique<Chess::Engine>(enginePath);
                updateGame();
            } else {
                // User canceled the file chooser.
                // Reset both players to "Human" since we don't have a valid engine.
                playerOptions.whiteMenu.setSelectedId(1, juce::sendNotification);
                playerOptions.blackMenu.setSelectedId(1, juce::sendNotification);
            }
        });
    }
}

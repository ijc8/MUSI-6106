#pragma once

#include <functional>

// We're using CMake, so we include JUCE module headers directly
// (as opposed using Projucer to generate one big JuceHeader.h).
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "BoardComponent.h"
#include "CommentarySonifier.h"
#include "EngineBridge.h"
#include "GameState.h"
#include "GameStream.h"
#include "StorySonifier.h"
#include "ThreatsSonifier.h"
#include "ZenSonifier.h"

// Helper classes to simplify MainComponent.
class Controls: public juce::GroupComponent {
public:
    Controls();
    void resized() override;

    juce::Label move;
    juce::ImageButton skipBackward, stepBackward, stepForward, skipForward, playPause;

    juce::Label autoAdvance{"", "Auto-advance every"};
    juce::TextEditor autoAdvancePeriod;
    juce::Label seconds{"", "seconds"};

    juce::ToggleButton pgnAdvance{"Use PGN clock times if available"};
};

class PlayerOptions: public juce::GroupComponent {
public:
    PlayerOptions();
    void resized() override;

    juce::Label whiteLabel, blackLabel;
    juce::ComboBox whiteMenu, blackMenu;
};

class SoundOptions: public juce::GroupComponent {
public:
    SoundOptions();
    void resized() override;
    double getGain() const;

    juce::Label sonifierLabel, volumeLabel;
    juce::ComboBox sonifierMenu;
    juce::Slider volumeSlider;
};

class AnalysisOptions: public juce::GroupComponent {
public:
    AnalysisOptions();
    void resized() override;

    juce::TextButton loadGame, streamGame;
    juce::Label fenLabel;
    juce::TextEditor fen;
};

class MainComponent: public juce::AudioAppComponent, public juce::Timer {
public:
    enum class PlayerType {
        Human,
        ComputerEasy,
        ComputerMedium,
        ComputerHard,
    };

    MainComponent();
    ~MainComponent();

    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override {};
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    // Timer
    void timerCallback() override;

    void updateGame();

private:
    void makeMove(Chess::Move move);
    bool undo();
    bool redo();
    void clearRedoStack();
    void setPlayerType(Chess::Color color, PlayerType type);
    void enableStockfish(bool shouldTurnOn);

    std::unique_ptr<Chess::Engine> engine;
    std::unique_ptr<juce::FileChooser> engineChooser;
    std::stack<Chess::Move> redoStack;
    Chess::Game &game = AppState::getInstance().getGame();

    PlayerType players[2] = {PlayerType::Human};

    double sampleRate;

    std::unique_ptr<Sonifier> oldSonifier, currentSonifier;

    struct SonifierType {
        std::string name;
        std::function<std::unique_ptr<Sonifier>(float)> create;
    };

    std::vector<SonifierType> sonifiers = {
        {"Zen", [](float sr) { return std::make_unique<ZenSonifier>(sr); }},
        {"Story", [](float sr) { return std::make_unique<StorySonifier>(sr); }},
        {"Threat", [](float sr) { return std::make_unique<ThreatsSonifier>(sr); }},
        {"Commentary", [](float sr) { return std::make_unique<CommentarySonifier>(sr); }}};

    BoardComponent board;

    Controls controls;
    PlayerOptions playerOptions;
    SoundOptions soundOptions;
    AnalysisOptions analysisOptions;

    juce::Label turnLabel;
    // std::vector<std::shared_ptr<GameStream>> streams;
    // std::shared_ptr<GameStream> stream;

    void loadSavedGame();

    std::unique_ptr<juce::FileChooser> fileChooser;
    // juce::String pgnData;

    void setSonifier(int sonifierIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

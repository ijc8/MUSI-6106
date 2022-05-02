#pragma once

#include <functional>

// We're using CMake, so we include JUCE module headers directly
// (as opposed using Projucer to generate one big JuceHeader.h).
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "BoardComponent.h"
#include "BroadcastManager.h"
#include "CommentarySonifier.h"
#include "GameState.h"
#include "GameStream.h"
#include "StorySonifier.h"
#include "ThreatsSonifier.h"
#include "ZenSonifier.h"

class MainComponent: public juce::AudioAppComponent, public juce::ChangeListener {
public:
    enum GameMode {
        PVP,
        PVC,
        PGN
    };

    MainComponent();
    ~MainComponent();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override{};

    void paint(juce::Graphics &g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

private:
    double sampleRate;

    GameMode mode = PVP;
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

    BroadcastManager broadcastManager;
    BoardComponent board;

    juce::TextButton undo;
    juce::TextButton redo;

    juce::Label sonifierLabel;
    juce::ComboBox sonifierMenu;

    juce::Label modeLabel;
    juce::ComboBox modeMenu;

    juce::Slider volumeSlider;

    juce::Label streamInputLabel;
    juce::TextEditor streamInput;
    juce::TextButton streamToggle;
    std::vector<std::shared_ptr<GameStream>> streams;
    std::shared_ptr<GameStream> stream;
    std::future<void> task;

    juce::Label fenLabel;
    juce::Label fenInput;
    juce::Label turnLabel;

    juce::TextButton openPGN;
    juce::TextButton nextButton;
    juce::TextButton prevButton;

    void onFenChanged();

    void onPgnButtonClicked();
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::String pgnData;

    void onGameModeChange(MainComponent::GameMode nextGameMode);
    void setSonifier(int sonifierIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

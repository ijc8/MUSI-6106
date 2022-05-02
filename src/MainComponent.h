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

    class GameSetup: public juce::GroupComponent {
    public:
        GameSetup() {
            setText("Game Setup");
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

        void resized() override {
            juce::FlexBox fb;
            fb.flexDirection = juce::FlexBox::Direction::column;
            fb.justifyContent = juce::FlexBox::JustifyContent::center;
            fb.alignContent = juce::FlexBox::AlignContent::center;
            // Order matches board orientation.
            fb.items.add(juce::FlexItem(blackMenu).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
            fb.items.add(juce::FlexItem(whiteMenu).withMinHeight(30).withMargin(juce::FlexItem::Margin(24, 12, 6, 12)));
            fb.performLayout(getLocalBounds());
        }

        juce::Label whiteLabel, blackLabel;
        juce::ComboBox whiteMenu, blackMenu;
    };

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

    GameSetup gameSetup;

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

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

    GameMode m_GameMode = PVP;
    std::unique_ptr<Sonifier> mOldSonifier, mCurrentSonifier;

    struct SonifierType {
        std::string name;
        std::function<std::unique_ptr<Sonifier>(float)> create;
    };

    std::vector<SonifierType> sonifiers = {
        {"Zen", [](float sr) { return std::make_unique<ZenSonifier>(sr); }},
        {"Story", [](float sr) { return std::make_unique<StorySonifier>(sr); }},
        {"Threat", [](float sr) { return std::make_unique<ThreatsSonifier>(sr); }},
        {"Commentary", [](float sr) { return std::make_unique<CommentarySonifier>(sr); }}};

    BroadcastManager m_BroadcastManager;
    BoardComponent m_ChessboardGUI;

    juce::TextButton buttonUndo;
    juce::TextButton buttonRedo;
    juce::Label sonifierLabel;
    juce::ComboBox m_SonifierSelector;
    juce::Label modeLabel;
    juce::ComboBox m_GameModeSelector;
    juce::Slider m_VolumeSlider;

    juce::Label streamInputLabel;
    juce::TextEditor streamInput;
    juce::TextButton streamToggle;
    std::unique_ptr<GameStream> stream;
    std::future<void> task;

    juce::Label m_FenLabel;
    juce::Label m_FenInput;
    juce::Label m_TurnText;
    juce::TextButton m_pgnButton;
    juce::TextButton m_NextButton;
    juce::TextButton m_PrevButton;

    void onFenChanged();

    void onPgnButtonClicked();
    std::unique_ptr<juce::FileChooser> m_FileChooser;
    juce::String m_PgnString;

    void onGameModeChange(MainComponent::GameMode nextGameMode);
    void setSonifier(int sonifierIndex);
    std::stack<Chess::Move> mUndoHistory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

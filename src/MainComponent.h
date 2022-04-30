#pragma once

#include <functional>

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "BoardComponent.h"
#include "DebugSonifier.h"
#include "ThreatsSonifier.h"
#include "StorySonifier.h"
#include "GameState.h"
#include "BroadcastManager.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent, public juce::ChangeListener
{
public:

    enum GameMode {
        PVP,
        PVC,
        PGN
    };

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override {};

    void paint(juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    int samplesPerBlockExpected;
    double sampleRate;

    GameMode m_GameMode = PVP;
    std::unique_ptr<Sonifier> mOldSonifier, mCurrentSonifier;

    struct SonifierType {
        std::string name;
        std::function<std::unique_ptr<Sonifier>()> create;
    };

    std::vector<SonifierType> sonifiers = {
        {"Debug", [](){ return std::make_unique<DebugSonifier>(); }},
        {"Threat", [](){ return std::make_unique<ThreatsSonifier>(); }},
        {"Story", [](){ return std::make_unique<StorySonifier>(); }},
    };

    BroadcastManager m_BroadcastManager;
    BoardComponent m_ChessboardGUI;

    juce::TextButton buttonUndo;
    juce::TextButton buttonRedo;
    juce::ComboBox m_SonifierSelector;
    juce::ComboBox m_GameModeSelector;
    juce::Slider m_VolumeSlider;

    juce::Label m_TurnText;
    juce::TextButton m_pgnButton;
    juce::TextButton m_NextButton;
    juce::TextButton m_PrevButton;

    void onPgnButtonClicked();
    std::unique_ptr<juce::FileChooser> m_FileChooser;
    juce::String m_PgnString;

    void onGameModeChange(MainComponent::GameMode nextGameMode);
    void setSonifier(int sonifierIndex);
    std::stack<Chess::Move> mUndoHistory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

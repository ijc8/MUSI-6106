#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "ChessboardGUI.h"
#include "DebugSonifier.h"
#include "GameState.h"
#include "BroadcastManager.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:

    enum class GameMode {
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
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    MainComponent::GameMode m_GameMode = MainComponent::GameMode::PVP;

    BroadcastManager m_BroadcastManager;
    DebugSonifier m_DebugSonifier;
    GUI::ChessBoard m_ChessboardGUI;
    juce::TextButton buttonPreset1;
    juce::TextButton buttonPreset2;
    juce::TextButton buttonPreset3;
    juce::TextButton buttonPreset4;
    juce::TextButton buttonPreset5;
    juce::TextButton buttonReset;
    juce::ComboBox m_SonifierSelector;
    juce::ComboBox m_GameModeSelector;

    juce::Label m_TitleText;
    juce::TextButton m_pgnButton;
    juce::TextButton m_NextButton;
    juce::TextButton m_PrevButton;

    void onSonifierChange();

    void onPgnButtonClicked();
    std::unique_ptr<juce::FileChooser> m_FileChooser;
    juce::String m_PgnString;

    void onGameModeChange(MainComponent::GameMode nextGameMode);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

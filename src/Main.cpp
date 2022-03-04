/*
  ==============================================================================

    This file contains the startup code for a PIP.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ImagesDemo.h"
#include "GridDemo.h"
#include<map>
#include<sstream>
#include <iostream>
#include<string.h>
#include <stdio.h>
using namespace std;
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

#pragma once

class Application    : public juce::JUCEApplication
{
public:
    //==============================================================================
    Application() = default;

    const juce::String getApplicationName() override       { return "Chess_Board"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }

    

    void initialise (const juce::String&) override
    {
        std::map<int, const char*> board_state;
        for (int j = 0; j <= 7; j++) {
            for (int w = 2; w <= 5; w++) {
                board_state[j*8+w] = "";
            }
            board_state[j * 8 + 1] = "B_Pawn.png";
            board_state[j * 8 + 6] = "W_Pawn.png";
        }
        board_state[0] = "B_Rook.png";
        board_state[0+8*7] = "B_Rook.png";
        board_state[7] = "W_Rook.png";
        board_state[7+8*7] = "W_Rook.png";
        board_state[0+8] = "B_Knight.png";
        board_state[0 + 8*6] = "B_Knight.png";
        board_state[7+8] = "W_Knight.png";
        board_state[7 + 8*6] = "W_Knight.png";
        board_state[0 + 8*2] = "B_Bishop.png";
        board_state[0 + 8 * 5] = "B_Bishop.png";
        board_state[7 + 8*2] = "W_Bishop.png";
        board_state[7 + 8 * 5] = "W_Bishop.png";
        board_state[0 + 8 * 3] = "B_Queen.png";
        board_state[0 + 8 * 4] = "B_King.png";
        board_state[7 + 8 * 3] = "W_Queen.png";
        board_state[7 + 8 * 4] = "W_King.png";

        time_t now = time(0);
        int time_0 = static_cast<int>(now);
        tm* ltm = localtime(&now);
        int ltm_second = ltm->tm_sec;
        char output = static_cast<char>(ltm_second);
        char* output_path = &output;
        board_state[64] = "time:0";
        board_state[65] = "black_turn";
        mainWindow.reset(new MainWindow("Chess", new GridDemo(board_state), *this));

        time_t first = time(NULL);

        while (1)

        {
            if (KEY_DOWN(MOUSE_MOVED)) {
                
                board_state[64] = ctime(&now);
                if (board_state[65] == "black_turn") {
                    board_state[65] = "white_turn";
                }
                else {
                    board_state[65] = "black_turn";
                }
                if (board_state[6]=="W_Pawn.png") {
                    board_state[6] = "";
                    board_state[5] = "W_Pawn.png";
                }
                else {
                    board_state[5] = "";
                    board_state[6] = "W_Pawn.png";
                }

                mainWindow.reset(new MainWindow("Chess", new GridDemo(board_state), *this));
            }
            /*
            time_t now = time(0);
            int time_0 = static_cast<int>(now);
            tm* ltm = localtime(&now);
            int ltm_second = ltm->tm_sec;
            char output = static_cast<char>(ltm_second);
            char* output_path = &output;
            board_state[64] = ctime(&now);
            */
            time_t second = time(NULL);
            char stringform[3];
            int differ = round(difftime(second, first));
            itoa(differ, stringform, 10);
            board_state[64] = "time:" + stringform[0];
        }
    }

    

    void shutdown() override                         { mainWindow = nullptr; }

private:
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name, juce::Component* c, JUCEApplication& a)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                .findColour (ResizableWindow::backgroundColourId),
                              juce::DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================

START_JUCE_APPLICATION(Application)


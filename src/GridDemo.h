/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             GridDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Responsive layouts using Grid.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2019, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        GridDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "DemoUtilities.h"
#include<windows.h>
#include<cstdio>
#include<map>
#include <ctime>
#include <time.h>


#pragma once

//==============================================================================
class GridDemo   : public Component
{
public:
    //const juce::String getName() const override { return "Arpeggiator"; };

    
    GridDemo(std::map<int,const char*> board_state)
    {
        

        //
        ImageComponent imagePreview;
        //juce::File select("C:\\ASE\\project\\GUI\\GUI_BUILD\\Chess\\GridDemo\\Builds\\VisualStudio2019\\B_Bishop.png");
        auto select= juce::File("C:\\ASE\\project\\GUI\\GUI_BUILD\\Chess\\GridDemo\\Builds\\VisualStudio2019\\B_Bishop.png");
        imagePreview.setImage(ImageCache::getFromFile(select));
        juce::Image x=Image(Image::ARGB, 100, 100, true);
        x=imagePreview.getImage();
        juce::Rectangle<int> y = imagePreview.getImage().getBounds();


        //

        int count = 0;
        for (int i = 0; i <= 63; i++) {
            if ((static_cast<int>(count/8)) % 2 == 0) {
                if (i % 2 == 0) {
                    addGridItemPanel(Colours::white, board_state[i], imagePreview);
                }
                else {
                    addGridItemPanel(Colours::black, board_state[i], imagePreview);
                }
            }
            else {
                if ((i+1) % 2 == 0) {
                    addGridItemPanel(Colours::white, board_state[i], imagePreview);
                }
                else {
                    addGridItemPanel(Colours::black, board_state[i], imagePreview);
                }
            }
            count++;
        }
        
        

        
        addGridItemPanel(Colours::burlywood, board_state[64], imagePreview);

        addGridItemPanel(Colours::burlywood, board_state[65], imagePreview);
        
        
        setSize (720, 640);

        //==============================================================================
        

    

    }

    

    void addGridItemPanel (Colour colour, const char* text, ImageComponent& imagePreview)
    {
        addAndMakeVisible (items.add (new GridItemPanel (colour, text, imagePreview)));
        addAndMakeVisible(imagePreview);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::burlywood);


        

        //imagePreview.setImage(ImageCache::getFromFile(selectedFile));
    }

    void resized() override
    {
        Grid grid;

        grid.rowGap    = 0_px;
        grid.columnGap = 0_px;

        using Track = Grid::TrackInfo;

        grid.templateRows = { Track (1_fr), Track (1_fr), Track (1_fr) ,Track (1_fr), Track (1_fr), Track (1_fr),Track (1_fr), Track (1_fr)};

        grid.templateColumns = { Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr),
                                 Track (1_fr)};


        grid.autoColumns = Track (1_fr);
        grid.autoRows    = Track (1_fr);

        grid.autoFlow = Grid::AutoFlow::column;

        for (int i = 0; i <= 65; i++) {
            grid.items.addArray({ GridItem(items[i]) });
        }
        

        auto r = getLocalBounds().reduced(4);

        

        grid.performLayout (getLocalBounds());

        //int x;
        //x = MessageBox(GetForegroundWindow(), "Black Turn!", "Message", 1);
    }

   

    

    //==============================================================================
    struct GridItemPanel  : public Component
    {
        GridItemPanel (Colour colourToUse, const String& textToUse,const ImageComponent& imagePreview)
            : colour (colourToUse),
              text (textToUse),
            imageGrid(imagePreview)
        {}

        void paint (Graphics& g) override
        {
            
            g.fillAll (colour.withAlpha (0.5f));

            g.setColour (Colours::black);
            //g.drawText (text, getLocalBounds().withSizeKeepingCentre (100, 100), Justification::centred, false);
            if (text != "")
            {
                if (text[text.length() - 3] == 'p' && text[text.length() - 2] == 'n' && text[text.length() - 1] == 'g')
                {
                    juce::Rectangle<int> r = getLocalBounds().withSizeKeepingCentre(75, 75);
                    ImageComponent imageGrid;
                    auto select = juce::File("C:\\ASE\\project\\GUI\\GUI_BUILD\\Chess\\GridDemo\\Builds\\VisualStudio2019\\" + text);
                    imageGrid.setImage(ImageCache::getFromFile(select));
                    juce::Image x = Image(Image::RGB, 60, 60, false);
                    x = imageGrid.getImage();
                    //
                    g.drawImage(x, juce::Rectangle<float>(static_cast<float>(r.getX()), static_cast<float>(r.getY()), static_cast<float>(r.getWidth()), static_cast<float>(r.getHeight())), imageGrid.getImagePlacement(), false);

                }
                else {
                    g.drawText (text, getLocalBounds().withSizeKeepingCentre (100, 100), Justification::centred, false);

                }
            }

           
        }

        


        
        //
        
        //

        const ImageComponent& imageGrid;
        Colour colour;
        String text;
    };

    OwnedArray<GridItemPanel> items;
};

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

//==============================================================================
struct GridDemo   : public Component
{
    GridDemo()
    {
        addGridItemPanel (Colours::white, "0");
        addGridItemPanel (Colours::black,        "1");
        addGridItemPanel (Colours::white,       "2");
        addGridItemPanel (Colours::black,      "3");
        addGridItemPanel (Colours::white,     "4");
        addGridItemPanel (Colours::black,      "5");
        addGridItemPanel (Colours::white, "6");
        addGridItemPanel (Colours::black,        "7");
        addGridItemPanel (Colours::black,       "8");
        addGridItemPanel (Colours::white,      "9");
        addGridItemPanel (Colours::black,     "10");
        addGridItemPanel (Colours::white,      "11");
        addGridItemPanel (Colours::black,      "12");
        addGridItemPanel (Colours::white,        "13");
        addGridItemPanel (Colours::black,       "14");
        addGridItemPanel (Colours::white,      "15");
        addGridItemPanel (Colours::white,     "16");
        addGridItemPanel (Colours::black,      "17");
        addGridItemPanel (Colours::white, "18");
        addGridItemPanel (Colours::black,        "19");
        addGridItemPanel (Colours::white,       "20");
        addGridItemPanel (Colours::black,      "21");
        addGridItemPanel (Colours::white,     "22");
        addGridItemPanel (Colours::black,      "23");
        addGridItemPanel (Colours::black,      "24");
        addGridItemPanel (Colours::white,        "25");
        addGridItemPanel (Colours::black,       "26");
        addGridItemPanel (Colours::white,      "27");
        addGridItemPanel (Colours::black,     "28");
        addGridItemPanel (Colours::white,      "29");
        addGridItemPanel (Colours::black, "30");
        addGridItemPanel (Colours::white,        "31");
        addGridItemPanel (Colours::white,       "32");
        addGridItemPanel (Colours::black,      "33");
        addGridItemPanel (Colours::white,     "34");
        addGridItemPanel (Colours::black,      "35");
        addGridItemPanel (Colours::white,      "36");
        addGridItemPanel (Colours::black,        "37");
        addGridItemPanel (Colours::white,       "38");
        addGridItemPanel (Colours::black,      "39");
        addGridItemPanel (Colours::black,     "40");
        addGridItemPanel (Colours::white,      "41");
        addGridItemPanel (Colours::black, "42");
        addGridItemPanel (Colours::white,        "43");
        addGridItemPanel (Colours::black,       "44");
        addGridItemPanel (Colours::white,      "45");
        addGridItemPanel (Colours::black,     "46");
        addGridItemPanel (Colours::white,      "47");
        addGridItemPanel (Colours::white,      "48");
        addGridItemPanel (Colours::black,      "49");
        addGridItemPanel (Colours::white, "50");
        addGridItemPanel (Colours::black,        "51");
        addGridItemPanel (Colours::white,       "52");
        addGridItemPanel (Colours::black,      "53");
        addGridItemPanel (Colours::white,     "54");
        addGridItemPanel (Colours::black,      "55");
        addGridItemPanel (Colours::black,      "56");
        addGridItemPanel (Colours::white,        "57");
        addGridItemPanel (Colours::black,       "58");
        addGridItemPanel (Colours::white,      "59");
        addGridItemPanel (Colours::black,     "60");
        addGridItemPanel (Colours::white,      "61");
        addGridItemPanel (Colours::black, "62");
        addGridItemPanel (Colours::white,        "63");
        addGridItemPanel (Colours::white,       "64");

        setSize (640, 640);
    }

    void addGridItemPanel (Colour colour, const char* text)
    {
        addAndMakeVisible (items.add (new GridItemPanel (colour, text)));
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
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

        grid.items.addArray ({ GridItem (items[0]),//.withArea (2, 2, 4, 4),
                               GridItem (items[1]),
                               GridItem (items[2]),//.withArea ({}, 3),
                               GridItem (items[3]),
                               GridItem (items[4]),//withArea (GridItem::Span (2), {}),
                               GridItem (items[5]),
                               GridItem (items[6]),
                               GridItem (items[7]),
                               GridItem (items[8]),
                               GridItem (items[9]),
                               GridItem (items[10]),
                               GridItem (items[11]),
                               GridItem (items[12]),
                               GridItem (items[13]),
                               GridItem (items[14]),
                               GridItem (items[15]),
                               GridItem (items[16]),
                               GridItem (items[17]),
                               GridItem (items[18]),
                               GridItem (items[19]),
                               GridItem (items[20]),
                               GridItem (items[21]),
                               GridItem (items[22]),
                               GridItem (items[23]),
                               GridItem (items[24]),
                               GridItem (items[25]),
                               GridItem (items[26]),
                               GridItem (items[27]),
                               GridItem (items[28]),
                               GridItem (items[29]),
                               GridItem (items[30]),
                               GridItem (items[31]),
                               GridItem (items[32]),
                               GridItem (items[33]),
                               GridItem (items[34]),
                               GridItem (items[35]),
                               GridItem (items[36]),
                               GridItem (items[37]),
                               GridItem (items[38]),
                               GridItem (items[39]),
                               GridItem (items[40]),
                               GridItem (items[41]),
                               GridItem (items[42]),
                               GridItem (items[43]),
                               GridItem (items[44]),
                               GridItem (items[45]),
                               GridItem (items[46]),
                               GridItem (items[47]),
                               GridItem (items[48]),
                               GridItem (items[49]),
                               GridItem (items[50]),
                               GridItem (items[51]),
                               GridItem (items[52]),
                               GridItem (items[53]),
                               GridItem (items[54]),
                               GridItem (items[55]),
                               GridItem (items[56]),
                               GridItem (items[57]),
                               GridItem (items[58]),
                               GridItem (items[59]),
                               GridItem (items[60]),
                               GridItem (items[61]),
                               GridItem (items[62]),
                               GridItem (items[63])
                            });

        grid.performLayout (getLocalBounds());
    }

    //==============================================================================
    struct GridItemPanel  : public Component
    {
        GridItemPanel (Colour colourToUse, const String& textToUse)
            : colour (colourToUse),
              text (textToUse)
        {}

        void paint (Graphics& g) override
        {
            g.fillAll (colour.withAlpha (0.5f));

            g.setColour (Colours::black);
            g.drawText (text, getLocalBounds().withSizeKeepingCentre (100, 100), Justification::centred, false);
        }

        Colour colour;
        String text;
    };

    OwnedArray<GridItemPanel> items;
};

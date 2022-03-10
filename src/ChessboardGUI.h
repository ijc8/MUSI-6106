#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "GameState.h"

namespace GUI
{
	class Square : public juce::Button
	{
	public:
		Square(int row, int column, juce::Colour color) : juce::Button("square"), m_SquareColor(color), m_Row(row), m_Col(column)
		{
			m_Rank = toRank(row);
			m_File = toFile(column);
			setId(m_Rank, m_File);
			setSize(80, 80);
		};

		~Square() = default;

		void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
		{
			if (shouldDrawButtonAsHighlighted)
				g.fillAll(juce::Colours::yellow);
			else
				g.fillAll(m_SquareColor);
		}

		void resized() override
		{
			setBounds(getBounds());
		}

		uint8_t getRank() const { return m_Rank; };
		uint8_t getFile() const { return m_File; };
		juce::String getId() const { return m_Id; };

	private:

		uint8_t toRank(int row)
		{
			switch (row)
			{
			case 0: return '1';
			case 1: return '2';
			case 2: return '3';
			case 3: return '4';
			case 4: return '5';
			case 5: return '6';
			case 6: return '7';
			default: return '8';
			}
		}

		uint8_t toFile(int col)
		{
			switch (col)
			{
			case 0: return 'a';
			case 1: return 'b';
			case 2: return 'c';
			case 3: return 'd';
			case 4: return 'e';
			case 5: return 'f';
			case 6: return 'g';
			default: return 'h';
			}
		}

		void setId(uint8_t rank, uint8_t file)
		{
			m_Id = juce::String::charToString(file) + juce::String::charToString(rank);
		}

		int m_Row;
		int m_Col;
		uint8_t m_Rank;
		uint8_t m_File;
		juce::String m_Id;
		juce::Colour m_SquareColor;

	};
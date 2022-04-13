#pragma once

#include <string>
#include <map>
#include <cassert>

class FREQ
{
public:

	static float noteToFreq(const std::string& note);

private:

	static const std::map<std::string, float> mToFreq;

};

class TEMPO
{
public:
	static float beatToSec(const float numBeats, const float bpm)
	{
		assert(numBeats >= 0 && bpm > 0);
		return numBeats / ( bpm / 60.0f);
	}
};


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


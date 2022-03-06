#include "catch.hpp"

#define _USE_MATH_DEFINES
#include <iostream>
#include <catch.hpp>
#include <math.h>

#include "../src/MainProcessor.h"

void genSine(float* ppfBuffer, float fFreq, float fGain, float fSampleRate, int iNumSamples)
{
	for (int sample = 0; sample < iNumSamples; sample++)
	{
		ppfBuffer[sample] = fGain * static_cast<float>(sin(2.0f * M_PI * fFreq * sample / fSampleRate));
	}
}

void CHECK_ARRAY_CLOSE(float** ppfBuffer1, float** ppfBuffer2, int iNumChannels, int iNumSamples, float fTolerance)
{
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		for (int sample = 0; sample < iNumSamples; sample++)
		{
			float fDiff = ppfBuffer1[channel][sample] - ppfBuffer2[channel][sample];
			REQUIRE(abs(fDiff) < fTolerance);
		}
	}
}

TEST_CASE("Parameter Setting", "[synthesis]")
{
	CSineWavetable sine;
	CWavetableOscillator osc(sine);

	// Checks default parameter values
	REQUIRE(osc.getFrequency() == 0);
	REQUIRE(osc.getGain() == 0);
	REQUIRE(osc.getPan() == 0.5);

	osc.setFrequency(440);
	osc.setGain(0.5);
	osc.setPan(0.0f);
	osc.setSampleRate(44100);
	
	// Checks if valid parameters are correctly set
	REQUIRE(osc.getFrequency() == 440);
	REQUIRE(osc.getGain() == 0.5);
	REQUIRE(osc.getPan() == 0.0);
	REQUIRE(osc.getSampleRate() == 44100);

	// Tests out of bounds parameter setting
	REQUIRE(osc.setFrequency(-50) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setFrequency(40000000) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setGain(-23) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setGain(234) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setSampleRate(-23) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setPan(-24) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.setPan(2) == Error_t::kFunctionInvalidArgsError);

	// Ensures valid parameters are still set
	REQUIRE(osc.getFrequency() == 440);
	REQUIRE(osc.getGain() == 0.5);
	REQUIRE(osc.getPan() == 0.0);
	REQUIRE(osc.getSampleRate() == 44100);

}

TEST_CASE("CWavetableOscillator Accuracy", "[synthesis]")
{
	int iNumChannels = 2;
	int iNumFrames = 10000;
	float** ppfGroundBuffer = new float* [iNumChannels];
	float** ppfOscBuffer = new float* [iNumChannels];
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		ppfGroundBuffer[channel] = new float[iNumFrames] {0};
		ppfOscBuffer[channel] = new float[iNumFrames] {0};
	}

	CSineWavetable sineWavetable;
	CWavetableOscillator wavetableOscillator(sineWavetable);

	SECTION("Check different frequencies")
	{
		float fFreq = 440;
		float fGain = 0.5f;
		float fSampleRate = 44100;

		for (int channel = 0; channel < iNumChannels; channel++)
		{
			genSine(ppfGroundBuffer[channel], fFreq, fGain / 2.0f, fSampleRate, iNumFrames);
		}

		wavetableOscillator.setFrequency(fFreq);
		wavetableOscillator.setGain(fGain);
		wavetableOscillator.setSampleRate(fSampleRate);
		wavetableOscillator.setADSRParameters(0, 0, 1, 0);
		wavetableOscillator.noteOn();
		for (int frame = 0; frame < iNumFrames; frame++)
		{
			wavetableOscillator.process(ppfOscBuffer, iNumChannels, frame);
		}

		CHECK_ARRAY_CLOSE(ppfOscBuffer, ppfGroundBuffer, iNumChannels, iNumFrames, 1E-3);
	}

	for (int channel = 0; channel < iNumChannels; channel++)
	{
		delete[] ppfGroundBuffer[channel];
		delete[] ppfOscBuffer[channel];
	}
	delete[] ppfGroundBuffer;
	delete[] ppfOscBuffer;
}
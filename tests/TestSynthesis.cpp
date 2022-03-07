#include "catch.hpp"

#define _USE_MATH_DEFINES
#include <iostream>
#include <catch.hpp>
#include <math.h>

#include "../src/MainProcessor.h"

void genSine(float** ppfBuffer, float fFreq, float fGain, float fPan, float fSampleRate, int iNumChannels, int iNumSamples)
{
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		float fChannelPan = (channel == 0) ? (1.0f - fPan) : fPan;
		for (int sample = 0; sample < iNumSamples; sample++)
		{
			ppfBuffer[channel][sample] = fChannelPan * fGain * static_cast<float>(sin(2.0f * M_PI * fFreq * sample / fSampleRate));
		}
	}

}

void genOsc(CWavetableOscillator* pOsc, float** ppfBuffer, float fFreq, float fGain, float fPan, float fSampleRate, int iNumChannels, int iNumFrames)
{
	pOsc->setFrequency(fFreq);
	pOsc->setGain(fGain);
	pOsc->setPan(fPan);
	pOsc->setSampleRate(fSampleRate);
	pOsc->setADSRParameters(0, 0, 1, 0);
	pOsc->noteOn();
	for (int frame = 0; frame < iNumFrames; frame++)
	{
		pOsc->process(ppfBuffer, iNumChannels, frame);
	}
	pOsc->reset();
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

TEST_CASE("Parameter Setting", "[CWavetableOscillator]")
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

TEST_CASE("CWavetableOscillator Accuracy", "[CWavetableOscillator]")
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
	CWavetableOscillator* osc = new CWavetableOscillator(sineWavetable);

	SECTION("Check Various Frequencies")
	{
		std::vector<float> fFreqs{ 440, 220, 110, 500, 700, 1000};
		float fGain = 1.0f;
		float fPan = 0.5f;
		float fSampleRate = 44100;

		for (float fFreq : fFreqs)
		{
			genOsc(osc, ppfOscBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);
			genSine(ppfGroundBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);

			CHECK_ARRAY_CLOSE(ppfOscBuffer, ppfGroundBuffer, iNumChannels, iNumFrames, 1E-3);
			for (int channel = 0; channel < iNumChannels; channel++)
			{
				memset(ppfOscBuffer[channel], 0, sizeof(float) * iNumFrames);
				memset(ppfGroundBuffer[channel], 0, sizeof(float) * iNumFrames);
			}
		}
	}

	SECTION("Check Various Gains")
	{
		float fFreq = 440.0f;
		std::vector<float> fGains{ 1.0f, 0.5f, 0.25f, 0.0f, -0.25, -0.5, -1.0f };
		float fPan = 0.5f;
		float fSampleRate = 44100;

		for (float fGain : fGains)
		{
			genOsc(osc, ppfOscBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);
			genSine(ppfGroundBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);

			CHECK_ARRAY_CLOSE(ppfOscBuffer, ppfGroundBuffer, iNumChannels, iNumFrames, 1E-3);
			for (int channel = 0; channel < iNumChannels; channel++)
			{
				memset(ppfOscBuffer[channel], 0, sizeof(float) * iNumFrames);
				memset(ppfGroundBuffer[channel], 0, sizeof(float) * iNumFrames);
			}
		}
	}

	SECTION("Check Various Pans")
	{
		float fFreq = 440.0f;
		float fGain = 1.0f;
		std::vector<float> fPans{ 0.0f, 0.25f, 0.5f };
		float fSampleRate = 44100;

		for (float fPan : fPans)
		{
			genOsc(osc, ppfOscBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);
			genSine(ppfGroundBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);

			CHECK_ARRAY_CLOSE(ppfOscBuffer, ppfGroundBuffer, iNumChannels, iNumFrames, 1E-3);
			for (int channel = 0; channel < iNumChannels; channel++)
			{
				memset(ppfOscBuffer[channel], 0, sizeof(float) * iNumFrames);
				memset(ppfGroundBuffer[channel], 0, sizeof(float) * iNumFrames);
			}
		}
	}

	SECTION("Check Various Sample Rates")
	{
		float fFreq = 440.0f;
		float fGain = 1.0f;
		float fPan = 0.5f;
		std::vector<float> fSampleRates{ 44100, 22050, 11025 };

		for (float fSampleRate : fSampleRates)
		{
			genOsc(osc, ppfOscBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);
			genSine(ppfGroundBuffer, fFreq, fGain, fPan, fSampleRate, iNumChannels, iNumFrames);

			CHECK_ARRAY_CLOSE(ppfOscBuffer, ppfGroundBuffer, iNumChannels, iNumFrames, 1E-3);
			for (int channel = 0; channel < iNumChannels; channel++)
			{
				memset(ppfOscBuffer[channel], 0, sizeof(float) * iNumFrames);
				memset(ppfGroundBuffer[channel], 0, sizeof(float) * iNumFrames);
			}
		}
	}


	delete osc;
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		delete[] ppfGroundBuffer[channel];
		delete[] ppfOscBuffer[channel];
	}
	delete[] ppfGroundBuffer;
	delete[] ppfOscBuffer;
}

TEST_CASE("Shifting Parameters", "[CWavetableOscillator]")
{
	CSineWavetable sine;
	CWavetableOscillator osc(sine);

	osc.setFrequency(440);
	osc.setGain(0.5);

	osc.shiftFrequency(20);
	REQUIRE(osc.getFrequency() == 460);
	osc.shiftFrequency(-50);
	REQUIRE(osc.getFrequency() == 410);

	osc.shiftGain(0.25);
	REQUIRE(osc.getGain() == 0.75);
	osc.shiftGain(-1.0);
	REQUIRE(osc.getGain() == -0.25);

	REQUIRE(osc.shiftGain(500) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.shiftGain(-125) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.shiftFrequency(30000) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(osc.shiftFrequency(-30000) == Error_t::kFunctionInvalidArgsError);

}

TEST_CASE("Scheduler Testing", "[CScheduler]")
{
	int iNumChannels = 2;
	int iLength = 100000;

	float** ppfGroundBuffer = new float* [iNumChannels];
	float** ppfSchedulerBuffer = new float* [iNumChannels];
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		ppfGroundBuffer[channel] = new float[iLength] {0};
		ppfSchedulerBuffer[channel] = new float[iLength] {0};
	}

	float fFreq = 440;
	float fGain = 1.0f;
	float fPan = 0.5f;
	float fSampleRate = 44100;
	CSineWavetable sine;
	CWavetableOscillator* pOsc = new CWavetableOscillator(sine, fFreq, fGain, fSampleRate);
	CScheduler* pSchedule = new CScheduler(fSampleRate);

	SECTION("Triggers NoteOn() and NoteOff() Correctly")
	{
		float fOnsetInSec = 0.1f;
		float fDurationInSec = 0.5f;
		assert((fDurationInSec + fOnsetInSec) * fSampleRate < iLength);

		pSchedule->setADSRParameters(0, 0, 1, 0);
		pSchedule->pushInst(new CWavetableOscillator(sine, fFreq, fGain, fSampleRate), fOnsetInSec, fDurationInSec);

		int iNoteOn = fOnsetInSec * fSampleRate;
		int iNoteOff = (fDurationInSec - pOsc->getADSRParameters().release) * fSampleRate + iNoteOn;

		pSchedule->noteOn();
		for (int frame = 0; frame < iLength; frame++)
		{
			if (frame == iNoteOn)
				pOsc->noteOn();
			if (frame == iNoteOff)
				pOsc->noteOff();

			pOsc->process(ppfGroundBuffer, iNumChannels, frame);
			pSchedule->process(ppfSchedulerBuffer, iNumChannels, frame);
		}

		CHECK_ARRAY_CLOSE(ppfGroundBuffer, ppfSchedulerBuffer, iNumChannels, iLength, 1E-3);

	}

	delete pOsc;
	delete pSchedule;
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		delete[] ppfGroundBuffer[channel];
		delete[] ppfSchedulerBuffer[channel];
	}
	delete[] ppfGroundBuffer;
	delete[] ppfSchedulerBuffer;
}
#include "catch.hpp"

#include <iostream>
#include <catch.hpp>

#include "../src/MainProcessor.h"

TEST_CASE("Parameter Setting", "[synthesis]")
{
	CSineWavetable sine;
	CWavetableOscillator osc(sine);

	// Checks default parameter values
	REQUIRE(osc.getFrequency() == 0);
	REQUIRE(osc.getGain() == 0);
	REQUIRE(osc.getPan() == 0);

	osc.setFrequency(440);
	osc.setGain(0.5);
	osc.setPan(0.5f);
	osc.setSampleRate(44100);
	
	// Checks if valid parameters are correctly set
	REQUIRE(osc.getFrequency() == 440);
	REQUIRE(osc.getGain() == 0.5);
	REQUIRE(osc.getPan() == 0.5);
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
	REQUIRE(osc.getPan() == 0.5);
	REQUIRE(osc.getSampleRate() == 44100);

}
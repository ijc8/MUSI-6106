#include "catch.hpp"

#include <iostream>
#include <catch.hpp>

#include "DebugSonifier.h"

#include "GameState.h"

void CHECK_VALUE_CLOSE(float fVal1, float fVal2, float fTolerance)
{
	float fDiff = fVal1 - fVal2;
	REQUIRE(abs(fDiff) < fTolerance);
}

void CHECK_ARRAY_CLOSE(float** ppfBuffer1, float** ppfBuffer2, int iNumChannels, int iNumSamples, float fTolerance)
{
	for (int channel = 0; channel < iNumChannels; channel++)
	{
		for (int sample = 0; sample < iNumSamples; sample++)
		{
			CHECK_VALUE_CLOSE(ppfBuffer1[channel][sample], ppfBuffer2[channel][sample], fTolerance);
		}
	}
}

TEST_CASE("Zero Board as Input", "[zeroinput]") {
	DebugSonifier Sonifier;
	Chess::Board board1("8/8/8/8/8/8/8/8");
	Chess::Board board2("8/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	Chess::Board board3("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	Chess::Board board4("rnbqkbnr/pp3ppp/2ppp3/8/4P3/3B1N2/PPPP1PPP/RNBQK2R w KQkq - 0 1");
	Chess::Piece piece();

	float frequencies[8] = { 262.0f, 294.8f, 327.5f, 349.3f, 393.0f, 436.7f, 491.2f, 524.0f };
	float gains[8] = { 1.0f, 0.8f, 0.6f, 0.4f, 0.4f, 0.6f, 0.8f, 1.0f };
	float pans[8] = { 0.0f, 0.2f, 0.4f, 0.5f, 0.5f, 0.6f, 0.8f, 1.0f };
	int freqIdx = static_cast<int>(square.file);
	int gainIdx = static_cast<int>(square.rank);
	int panIdx = rand() % 8;

	REQUIRE(Sonifier.onMove(board1) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(Sonifier.onMove(board2) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(Sonifier.onMove(board3) == Error_t::kFunctionInvalidArgsError);
	REQUIRE(Sonifier.onMove(board4) == Error_t::kFunctionInvalidArgsError);


}
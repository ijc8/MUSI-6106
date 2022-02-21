#include "MainProcessor.h"

void MainProcessor::addInstRef(CInstrument& instrumentToAdd)
{
	setInsts.insert(&instrumentToAdd);
	updateGainNorm();
}

void MainProcessor::removeInstRef(CInstrument& instrumentToRemove)
{
	setInsts.erase(&instrumentToRemove);
	updateGainNorm();
}

void MainProcessor::pushInst(CInstrument* oscillatorToPush, float duration, float onset)
{
	Scheduler::pushInst(oscillatorToPush, duration, onset + sampToSec(sampleCounter, m_fSampleRateInHz));
	int deleteSample = secToSamp(onset + duration, m_fSampleRateInHz) + sampleCounter;
	mapRemover[deleteSample].insert(oscillatorToPush);
}

void MainProcessor::process(float** outBuffer, int numChannels, int numSamples, const int& masterClock)
{

	for (auto const& [sample, set] : mapRemover)
	{
		if (sample < sampleCounter)
		{
			for (CInstrument* inst : set)
			{
				setInsts.erase(inst);
			}
			mapRemover.erase(sample);
			if (mapRemover.empty()) break;
		}
	}

	Scheduler::process(outBuffer, numChannels, numSamples, masterClock);
}
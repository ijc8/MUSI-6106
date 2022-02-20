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

float MainProcessor::process()
{
	unordered_set removeSet = checkTriggers(sampleCounter, mapRemover);
	for (CInstrument* inst : removeSet)
		setInsts.erase(inst);

	return Scheduler::process();
}

unordered_set<CInstrument*> MainProcessor::checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck)
{
	auto triggerSample = mapToCheck.find(currentSample);
	if (triggerSample != mapToCheck.end())
	{
		unordered_set setToReturn = triggerSample->second;
		mapToCheck.erase(triggerSample);
		return setToReturn;
	}
	return unordered_set<CInstrument*>();
}
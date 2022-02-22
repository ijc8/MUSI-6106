#include "MainProcessor.h"

void MainProcessor::addInstRef(CInstrument& instrumentToAdd)
{
	setInsts.insert(&instrumentToAdd);
}

void MainProcessor::removeInstRef(CInstrument& instrumentToRemove)
{
	setInsts.erase(&instrumentToRemove);
}

void MainProcessor::addScheduleRef(Scheduler& scheduleToAdd)
{
	setSchedules.insert(&scheduleToAdd);
}

void MainProcessor::removeScheduleRef(Scheduler& scheduleToRemove)
{
	setSchedules.erase(&scheduleToRemove);
}

void MainProcessor::pushInst(CInstrument* oscillatorToPush, float duration, float onset)
{
	Scheduler::pushInst(oscillatorToPush, duration, onset + sampToSec(sampleCounter, m_fSampleRateInHz));
	int deleteSample = secToSamp(onset + duration, m_fSampleRateInHz) + sampleCounter;
	instRemover[deleteSample].insert(oscillatorToPush);
}

void MainProcessor::pushSchedule(Scheduler* scheduleToPush)
{
	setSchedules.insert(scheduleToPush);
	garbageCollector.insert(scheduleToPush);
	int deleteSample = scheduleToPush->getLength() + sampleCounter;
	scheduleRemover[deleteSample].insert(scheduleToPush);
}

void MainProcessor::pushLooper(Looper* loopToPush, int numTimesToLoop)
{
	setSchedules.insert(loopToPush);
	garbageCollector.insert(loopToPush);
	int deleteSample = loopToPush->getLength() * numTimesToLoop + sampleCounter;
	scheduleRemover[deleteSample].insert(loopToPush);
}

void MainProcessor::process(float** outBuffer, int numChannels, int numSamples, const int& masterClock)
{

	for (auto const& [sample, set] : instRemover)
	{
		if (sample < sampleCounter)
		{
			for (CInstrument* inst : set)
			{
				setInsts.erase(inst);
			}
			instRemover.erase(sample);
			if (instRemover.empty()) break;
		}
	}
	for (auto const& [sample, set] : scheduleRemover)
	{
		if (sample < sampleCounter)
		{
			for (Scheduler* schedule : set)
			{
				setSchedules.erase(schedule);
			}
			scheduleRemover.erase(sample);
			if (scheduleRemover.empty()) break;
		}
	}

	for (Scheduler* schedule : setSchedules)
		schedule->process(outBuffer, numChannels, numSamples, sampleCounter);
	Scheduler::process(outBuffer, numChannels, numSamples, sampleCounter);
}
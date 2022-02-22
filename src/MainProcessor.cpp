#include "MainProcessor.h"

void CMainProcessor::addInstRef(CInstrument& instrumentToAdd)
{
	setInsts.insert(&instrumentToAdd);
}

void CMainProcessor::removeInstRef(CInstrument& instrumentToRemove)
{
	setInsts.erase(&instrumentToRemove);
}

void CMainProcessor::addScheduleRef(CScheduler& scheduleToAdd)
{
	setSchedules.insert(&scheduleToAdd);
}

void CMainProcessor::removeScheduleRef(CScheduler& scheduleToRemove)
{
	setSchedules.erase(&scheduleToRemove);
}

void CMainProcessor::pushInst(CInstrument* oscillatorToPush, float duration, float onset)
{
	CScheduler::pushInst(oscillatorToPush, duration, onset + sampToSec(sampleCounter, m_fSampleRateInHz));
	int deleteSample = secToSamp(onset + duration, m_fSampleRateInHz) + sampleCounter;
	instRemover[deleteSample].insert(oscillatorToPush);
}

void CMainProcessor::pushSchedule(CScheduler* scheduleToPush)
{
	setSchedules.insert(scheduleToPush);
	garbageCollector.insert(scheduleToPush);
	int deleteSample = scheduleToPush->getLength() + sampleCounter;
	scheduleRemover[deleteSample].insert(scheduleToPush);
}

void CMainProcessor::pushLooper(CLooper* loopToPush, int numTimesToLoop)
{
	setSchedules.insert(loopToPush);
	garbageCollector.insert(loopToPush);
	int deleteSample = loopToPush->getLength() * numTimesToLoop + sampleCounter;
	scheduleRemover[deleteSample].insert(loopToPush);
}

void CMainProcessor::process(float** outBuffer, int numChannels, int numSamples, const int& masterClock)
{
	// removes any temporary instruments that are done processing
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

	// removes any temporary schedules/loops that are done processing
	for (auto const& [sample, set] : scheduleRemover)
	{
		if (sample < sampleCounter)
		{
			for (CScheduler* schedule : set)
			{
				setSchedules.erase(schedule);
			}
			scheduleRemover.erase(sample);
			if (scheduleRemover.empty()) break;
		}
	}

	for (CScheduler* schedule : setSchedules)
		schedule->process(outBuffer, numChannels, numSamples, sampleCounter);
	CScheduler::process(outBuffer, numChannels, numSamples, sampleCounter);
}
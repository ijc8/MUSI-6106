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
	scheduleToPush->start();
}

void CMainProcessor::pushLooper(CLooper* loopToPush, int numTimesToLoop)
{
	setSchedules.insert(loopToPush);
	garbageCollector.insert(loopToPush);
	int deleteSample = loopToPush->getLength() * numTimesToLoop + sampleCounter;
	scheduleRemover[deleteSample].insert(loopToPush);
	loopToPush->start();
}

void CMainProcessor::process(float** outBuffer, int numChannels, int numSamples, const int& masterClock)
{
	// removes any temporary instruments that are done processing
	auto upperBoundInst = instRemover.upper_bound(sampleCounter);
	for (auto it = instRemover.begin(); it != upperBoundInst; it++)
	{
		for (CInstrument* inst : it->second)
		{
			setInsts.erase(inst);
		}
	}
	instRemover.erase(instRemover.begin(), upperBoundInst);

	// removes any temporary schedules/loops that are done processing
	auto upperBoundSchedule = scheduleRemover.upper_bound(sampleCounter);
	for (auto it = scheduleRemover.begin(); it != upperBoundSchedule; it++)
	{
		for (CScheduler* schedule : it->second)
		{
			setSchedules.erase(schedule);
		}
	}
	scheduleRemover.erase(scheduleRemover.begin(), upperBoundSchedule);

	for (CScheduler* schedule : setSchedules)
		schedule->process(outBuffer, numChannels, numSamples, sampleCounter);
	CScheduler::process(outBuffer, numChannels, numSamples, sampleCounter);
}
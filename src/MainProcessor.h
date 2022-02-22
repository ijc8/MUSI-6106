#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

class MainProcessor : public Scheduler
{
public:
	MainProcessor() {};
	~MainProcessor() {};

	void addInstRef(CInstrument& instrumentToAdd);
	void removeInstRef(CInstrument& instrumentToRemove);

	void addScheduleRef(Scheduler& scheduleToAdd);
	void removeScheduleRef(Scheduler& scheduleToRemove);

	void pushInst(CInstrument* instrumentToPush, float duration = 1.0f, float onset = 0.0f) override;
	void pushSchedule(Scheduler* scheduleToPush);
	void pushLooper(Looper* loopToPush, int numTimesToLoop);

	void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock);

protected:

	map<int, unordered_set<CInstrument*>> instRemover;
	map<int, unordered_set<Scheduler*>> scheduleRemover;

	unordered_set<Scheduler*> setSchedules;

};

#endif 
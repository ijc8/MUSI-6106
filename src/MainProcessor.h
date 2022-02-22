#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

// To be used as a singleton
class CMainProcessor : public CScheduler
{
public:
	CMainProcessor() {};
	~CMainProcessor() {};

	// Use these to add pre-existing instruments to output stream
	// noteOn() and noteOff() are up to you to call
	// Should be used for chess pieces
	void addInstRef(CInstrument& instrumentToAdd);
	void removeInstRef(CInstrument& instrumentToRemove);

	// Use these to add pre-existing schedule/loop to output stream
	void addScheduleRef(CScheduler& scheduleToAdd);
	void removeScheduleRef(CScheduler& scheduleToRemove);

	// Adds a temporary instrument or schedule
	// Use for dynamically-allocated instances ( 'pushInst(new CWavetableOscillator(...))' )
	// Will handle deletion
	void pushInst(CInstrument* instrumentToPush, float duration = 1.0f, float onset = 0.0f) override;
	void pushSchedule(CScheduler* scheduleToPush);
	void pushLooper(CLooper* loopToPush, int numTimesToLoop);

	void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock);

private:

	map<int, unordered_set<CInstrument*>> instRemover;
	map<int, unordered_set<CScheduler*>> scheduleRemover;

	unordered_set<CScheduler*> setSchedules;

};

#endif 
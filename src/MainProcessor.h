#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

// To be used as a singleton
class CMainProcessor : public CScheduler
{
public:
	CMainProcessor() { m_bIsPlaying = true; };
	~CMainProcessor() {};

	// Use for chess pieces or other class member instruments
	// noteOn() and noteOff() calls will be up to you
	void addInstRef(CInstrument& rInstToAdd);
	void removeInstRef(CInstrument& rInstToRemove);

	// Use for any class member loops or schedules
	void addScheduleRef(CScheduler& rScheduleToAdd);
	void removeScheduleRef(CScheduler& rScheduleToRemove);

	// Use for dynamically-allocated instruments
	// Will handle deletion
	void pushInst(CInstrument* pInstToPush, float fOnsetInSec = 0.0f, float fDurationInSec = 1.0f) override;

	// Use for dynamically-allocated schedules
	// Will handle deletion
	void pushSchedule(CScheduler* pScheduleToPush);

	// Use for dynamically-allocated schedules
	// Will handle deletion
	void pushLooper(CLooper* pLoopToPush, int iNumTimesToLoop);

	void process(float** ppfOutBuffer, int iNumChannels, int iNumSamples, const int& iMasterClock);

private:

	map<int, unordered_set<CInstrument*>> m_InstRemover;
	map<int, unordered_set<CScheduler*>> m_ScheduleRemover;

	unordered_set<CScheduler*> m_SetSchedules;

};

#endif 
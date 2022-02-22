#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

// To be used as a singleton
class CMainProcessor : public CScheduler
{
public:
	CMainProcessor() { m_bIsPlaying = true; };
	~CMainProcessor() {};

	// Use these to add pre-existing instruments to output stream
	// noteOn() and noteOff() are up to you to call
	// Should be used for chess pieces
	void addInstRef(CInstrument& rInstToAdd);
	void removeInstRef(CInstrument& rInstToRemove);

	// Use these to add pre-existing schedule/loop to output stream
	void addScheduleRef(CScheduler& rScheduleToAdd);
	void removeScheduleRef(CScheduler& rScheduleToRemove);

	// Adds a temporary instrument or schedule
	// Use for dynamically-allocated instances ( 'pushInst(new CWavetableOscillator(...))' )
	// Will handle deletion
	void pushInst(CInstrument* pInstToPush, float fOnsetInSec = 0.0f, float fDurationInSec = 1.0f) override;
	void pushSchedule(CScheduler* pScheduleToPush);
	void pushLooper(CLooper* pLoopToPush, int iNumTimesToLoop);

	void process(float** ppfOutBuffer, int iNumChannels, int iNumSamples, const int& iMasterClock);

private:

	map<int, unordered_set<CInstrument*>> m_InstRemover;
	map<int, unordered_set<CScheduler*>> m_ScheduleRemover;

	unordered_set<CScheduler*> m_SetSchedules;

};

#endif 
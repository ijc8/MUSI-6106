#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

// To be used as a singleton
class CMainProcessor : public CScheduler
{
public:
	CMainProcessor() { noteOn(); };
	~CMainProcessor() {};

	// Use for chess pieces or other class member instruments
	// noteOn() and noteOff() calls will be up to you
	void addInstRef(CInstrument& rInstToAdd);
	void removeInstRef(CInstrument& rInstToRemove);

	// Use for dynamically-allocated instruments
	// Will handle deletion
	void pushInst(CInstrument* pInstToPush, float fOnsetInSec = 0.0f, float fDurationInSec = 1.0f) override;

	//void process(float** ppfOutBuffer, int iNumChannels, int iNumSamples) override;
	void process(float** ppfOutBuffer, int iNumChannels, int iNumFrames) override;

	static long long getMasterClock() { return s_iMasterClock; };

private:

	static long long s_iMasterClock;

	map<int, unordered_set<CInstrument*>> m_InstRemover;
	map<int, unordered_set<CScheduler*>> m_ScheduleRemover;

	unordered_set<CScheduler*> m_SetSchedules;

};

#endif 
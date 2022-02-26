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

	float getInternalClockInSeconds() { return m_iSampleCounter / m_fSampleRateInHz; };
	long long getInternalClockInSamples() { return m_iSampleCounter; };

protected:

	virtual unordered_set<CInstrument*> checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck);

};

#endif 
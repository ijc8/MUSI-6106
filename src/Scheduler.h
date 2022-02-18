#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__

#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <vector>

#include "SoundProcessor.h"

class CScheduler : public CSoundProcessor
{
public:
	CScheduler(float fSampleRate = 0.0f);
	~CScheduler();

	float process() override;

	Error_t add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t reset();

protected:

	int iCurrentSample = 0;
	std::unordered_map<int, std::unordered_set<CInstrument*>> m_ScheduleNoteOn;
	std::unordered_map<int, std::unordered_set<CInstrument*>> m_ScheduleNoteOff;
	std::forward_list<CInstrument*> m_InstrumentList;
	std::vector<CInstrument*> m_GarbageCollector;

	int convertSecToSamp(float fSec) const;
	Error_t addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnset, float fDurationInSec);

private:
	std::unordered_map<int, std::unordered_set<CInstrument*>> m_ScheduleRemover;
	Error_t addToInstRemover(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t addToSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
};

class CLooper : public CScheduler
{
public:
	CLooper(float fSampleRate = 0.0f);
	~CLooper();

	float process() override;

	Error_t add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t reset();

private:

	int iLoopSample = 1;

};
#endif // #if !defined(__Scheduler_hdr__)
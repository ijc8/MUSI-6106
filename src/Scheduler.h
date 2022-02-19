#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__
#include "SoundProcessor.h"

class Scheduler : public CSoundProcessor
{
public:
	Scheduler(float sampleRate = 48000) : CSoundProcessor(sampleRate) {};
	virtual ~Scheduler() {};

	virtual void pushInst(CInstrument* instrumentToPush, float duration, float onset);
	Error_t setSampleRate(float newSampleRate);

protected:

};

class Looper : public Scheduler
{
public:
	Looper(float sampleRate = 48000) : Scheduler(sampleRate) {};
	virtual ~Looper() {};

	virtual void pushInst(CInstrument* instrumentToPush, float duration, float onset) override;
	void setLoopLength(float newLoopLength);

protected:

private:

};








































//#include <map>
//#include <unordered_set>
//#include <forward_list>
//#include <vector>
//
//#include "SoundProcessor.h"
//
//class CLooper : public CSoundProcessor
//{
//public:
//	CLooper(float fSampleRate = 48000.0f);
//	~CLooper();
//
//	float process() override;
//
//	Error_t add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t reset();
//
//protected:
//
//	int iCurrentSample = 0;
//	std::map<int, std::unordered_set<CInstrument*>> m_ScheduleNoteOn;
//	std::map<int, std::unordered_set<CInstrument*>> m_ScheduleNoteOff;
//	std::forward_list<CInstrument*> m_InstrumentList;
//	std::vector<CInstrument*> m_GarbageCollector;
//
//	int convertSecToSamp(float fSec) const;
//	int convertSampToSec(int iSamp) const;
//	Error_t addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnset, float fDurationInSec);
//
//private:
//
//	int iLoopSample = 1;
//
//};
//
//class CScheduler : public CLooper
//{
//public:
//	CScheduler(float fSampleRate = 48000.0f);
//	~CScheduler();
//
//	float process() override;
//
//	Error_t add(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t add(CInstrument& rInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t reset();
//
//private:
//
//	std::map<int, std::unordered_set<CInstrument*>> m_ScheduleRemover;
//
//	Error_t addToADSRSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t addToInstRemover(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//	Error_t addToSchedulers(CInstrument* pInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
//};
//

#endif // #if !defined(__Scheduler_hdr__)
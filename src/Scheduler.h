#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__
#include "SoundProcessor.h"

#include <map>
#include <unordered_set>

using std::map;
using std::unordered_set;

// Class that contains a set of instruments scheduled for particular times and durations
class CScheduler : public CSoundProcessor
{
public:
	CScheduler(float sampleRate = 48000) : CSoundProcessor(sampleRate) {};
	virtual ~CScheduler();

	virtual void pushInst(CInstrument* instrumentToPush, float duration = 1.0f, float onset = 0.0f);
	void start();
	void stop();
	int getLength() const;

	virtual void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock);

protected:

	bool isPlaying = false;
	unordered_set<CInstrument*> setInsts;
	unordered_set<CSoundProcessor*> garbageCollector;
	long long sampleCounter = 0;
	int scheduleLength = 0;

};

class CLooper : public CScheduler
{
public:
	CLooper(float sampleRate = 48000) : CScheduler(sampleRate) {};
	~CLooper() = default;

	void setLoopLength(float newLoopLength);

	void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock);
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
#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__
#include "SoundProcessor.h"

#include <map>
#include <unordered_set>

using std::map;
using std::unordered_set;

// Class that contains a set of instruments scheduled for particular times and durations
class CScheduler : public CInstrument
{
public:
	CScheduler(float sampleRate = 48000);
	virtual ~CScheduler();

	// Schedule a dynamically-allocated instrument relative to the start of the container
	// Will handle deletion
	virtual void pushInst(CInstrument* pInstToPush, float fDurationInSec = 1.0f, float fOnsetInSec = 0.0f);

	// Resets and starts playback
	void noteOn() override;

	// Resets and stops playback
	void noteOff() override;

	// Returns schedule length in samples
	int getLength() const;

	virtual void process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;

protected:

	unordered_set<CInstrument*> m_SetInsts;
	unordered_set<CSoundProcessor*> m_GarbageCollector;
	long long m_iSampleCounter = 0;
	int m_iScheduleLength = 0;

	// Extra buffer space to apply adsr and gain
	const int m_iMaxChannels = 6;
	float** m_ppfTempBuffer = 0;


	map<int, unordered_set<CInstrument*>> m_MapNoteOn;
	map<int, unordered_set<CInstrument*>> m_MapNoteOff;
	map<int, unordered_set<CInstrument*>> m_MapRemover;

	virtual unordered_set<CInstrument*> checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck);
};

class CLooper : public CScheduler
{
public:
	CLooper(float sampleRate = 48000) : CScheduler(sampleRate) {};
	~CLooper() = default;

	void setLoopLength(float fNewLoopLengthInSec);

	void process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;
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
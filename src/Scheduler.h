#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__
#include "SoundProcessor.h"
#include "Ramp.h"
#include "AtomicRingBuffer.h"

#include <map>
#include <unordered_set>
#include <optional>


using std::map;
using std::unordered_set;

struct TriggerInfo
{
	TriggerInfo(int64_t noteOn, int64_t noteOff, int64_t remove) :
		noteOn(noteOn), noteOff(noteOff), remove(remove) {};
	int64_t noteOn = 0;
	int64_t noteOff = 0;
	int64_t remove = 0;
};

// Class that contains a set of instruments scheduled for particular times and durations
class CScheduler : public CInstrument
{
public:
	CScheduler(float sampleRate = 48000);
	virtual ~CScheduler();

	// Schedule a dynamically-allocated instrument relative to the start of the container
	// Will handle deletion
	virtual Error_t pushInst(CInstrument* pInstToPush, float fOnsetInSec = 0.0f, float fDurationInSec = 1.0f);

	virtual void noteOn() override;

	// Returns schedule length in samples
	int64_t getLengthInSamp() const;
	float getLengthInSec() const;

	// Frame-by-Frame processing function
	virtual void processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;

protected:

	// Set that contains instruments to be currently processed
	// Instruments will move in and out of this continuously
	unordered_set<CInstrument*> m_SetInsts;

	// All instrument pointers are placed and SHOULD STAY here to be deleted by the destructor
	unordered_set<CInstrument*> m_GarbageCollector;

	// This can be viewed as the schedule's internal clock
	int64_t m_iSampleCounter = 0;
	int64_t m_iScheduleLength = 0;

	// Extra buffer space for applying adsr and gain
	const int m_iMaxChannels = 20;
	float** m_ppfTempBuffer = 0;

	// These maps are used to trigger events on instruments at the correct sample
	// Key: Current Frame
	// Value: Set of Instruments with an event at that key
	map<int64_t, unordered_set<CInstrument*>> m_MapNoteOn;
	map<int64_t, unordered_set<CInstrument*>> m_MapNoteOff;
	map<int64_t, unordered_set<CInstrument*>> m_MapRemover;

	// Helper function to check if there is a trigger at a specified sample
	// Will return the set of instruments pertaining to the trigger if so
	// Will return an empty set if not
	virtual void checkTriggers();
	virtual void checkInsertQueue();
	
	juce::CriticalSection m_Lock;
	Ramp m_Ramp;
	AtomicRingBuffer<std::pair<CInstrument*, std::optional<TriggerInfo>>> m_InsertQueue{ 32 };
};

class CLooper : public CScheduler
{
public:
	CLooper(float sampleRate = 48000) : CScheduler(sampleRate) {};
	~CLooper() = default;

	Error_t setLoopLength(float fNewLoopLengthInSec);

	void processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;
protected:
	int m_iMinLoopLength = 0;
};
#endif
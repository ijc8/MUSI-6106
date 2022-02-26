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

	// Returns schedule length in samples
	int getLength() const;

	// Frame-by-Frame processing function
	virtual void process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;

protected:

	// Set that contains instruments to be currently processed
	// Instruments will move in and out of this continuously
	unordered_set<CInstrument*> m_SetInsts;

	// All instrument pointers are placed and SHOULD STAY here to be deleted by the destructor
	unordered_set<CSoundProcessor*> m_GarbageCollector;

	// This can be viewed as the schedule's internal clock
	long long m_iSampleCounter = 0;
	int m_iScheduleLength = 0;

	// Extra buffer space for applying adsr and gain
	const int m_iMaxChannels = 6;
	float** m_ppfTempBuffer = 0;

	// These maps are used to trigger events on instruments at the correct sample
	// Key: Current Frame
	// Value: Set of Instruments with an event at that key
	map<int, unordered_set<CInstrument*>> m_MapNoteOn;
	map<int, unordered_set<CInstrument*>> m_MapNoteOff;
	map<int, unordered_set<CInstrument*>> m_MapRemover;

	// Helper function to check if there is a trigger at a specified sample
	// Will return the set of instruments pertaining to the trigger if so
	// Will return an empty set if not
	virtual void checkTriggers();
};

class CLooper : public CScheduler
{
public:
	CLooper(float sampleRate = 48000) : CScheduler(sampleRate) {};
	~CLooper() = default;

	void setLoopLength(float fNewLoopLengthInSec);

	void process(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;
};
#endif
#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__
#include "SoundProcessor.h"
#include "AtomicRingBuffer.h"
#include "Util.h"

#include <map>
#include <unordered_set>
#include <optional>
#include <vector>
#include <string>


using std::map;
using std::unordered_set;

struct TriggerInfo
{
	TriggerInfo(int noteOn, int noteOff, int remove) :
		noteOn(noteOn), noteOff(noteOff), remove(remove) {};
	int noteOn = 0;
	int noteOff = 0;
	int remove = 0;
};

// Class that contains a set of instruments scheduled for particular times and durations
class CScheduler : public CInstrument
{
public:
	CScheduler(float sampleRate = 48000);
	virtual ~CScheduler();

	// Schedule a dynamically-allocated instrument relative to the start of the container
	Error_t scheduleInst(std::unique_ptr<CInstrument> pInstToPush, float fOnsetInSec, float fDurationInSec);
	Error_t scheduleTune(CWavetableOscillator osc, std::string notes[], float beats[], int numNotes, float bpm);
	Error_t scheduleChord(CWavetableOscillator osc, std::vector<std::string> notes, float LengthInBeats, float bpm);

	// Returns schedule length in samples
	int getLengthInSamp() const;
	float getLengthInSec() const;

	// Frame-by-Frame processing function
	virtual void processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;

	Error_t setSampleRate(float fSampleRate) override;

protected:

	// Set that contains instruments to be currently processed
	// Instruments will move in and out of this continuously
	unordered_set<std::shared_ptr<CInstrument>> m_ActiveInsts;

	// Set that contains all instruments that have been pushed
	unordered_set<std::shared_ptr<CInstrument>> m_AllInsts;

	// This can be viewed as the schedule's internal clock
	std::atomic<int64_t> m_iSampleCounter = 0;
	std::atomic<int> m_iScheduleLength = 0;

	// Extra buffer space for applying adsr and gain
	const int m_iMaxChannels = 20;
	float** m_ppfTempBuffer = 0;

	// These maps are used to trigger events on instruments at the correct sample
	// Key: Current Frame
	// Value: Set of Instruments with an event at that key
	map<int64_t, unordered_set<std::shared_ptr<CInstrument>>> m_MapNoteOn;
	map<int64_t, unordered_set<std::shared_ptr<CInstrument>>> m_MapNoteOff;
	map<int64_t, unordered_set<std::shared_ptr<CInstrument>>> m_MapRemover;

	// Checks for internal state changes i.e. noteOn() and/or noteOff() calls
	virtual void checkFlags() override;

	// Checks insert and remove queues
	virtual void checkQueues();

	// Parses each map and sees if any event triggers exist for child instrument at the current sample counter		
	// Carries out necessary actions if so
	virtual void checkTriggers();

	void updateSampleRate(map<int64_t, unordered_set<std::shared_ptr<CInstrument>>>& mapToUpdate, float fNewSampleRate);
	float updateSampleRate(float fValue, float fNewSampleRate);
	
	AtomicRingBuffer<std::pair<std::shared_ptr<CInstrument>, std::optional<TriggerInfo>>> m_InsertQueue{ 1000 };
private:
	CScheduler(const CScheduler& other);
};

class CLooper : public CScheduler
{
public:
	CLooper(float sampleRate = 48000) : CScheduler(sampleRate) {};
	virtual ~CLooper() = default;

	Error_t setLoopLength(float fNewLoopLengthInSec);

	void processFrame(float** ppfOutBuffer, int iNumChannels, int iCurrentFrame) override;
protected:
	int m_iMinLoopLength = 0;

private:
	CLooper(const CLooper& other);
};
#endif
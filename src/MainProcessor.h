#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

// To be used as a singleton
class CMainProcessor : public CScheduler, juce::Timer
{
public:
	CMainProcessor() { startTimer(500); };
	virtual ~CMainProcessor() { stopTimer(); };

	// Use when YOU want to externally control and modify the instrument being added
	// noteOn() and noteOff() calls will be up to you
	Error_t addInst(std::shared_ptr<CInstrument> pInstToAdd);
	Error_t removeInst(std::shared_ptr<CInstrument> pInstToRemove);

	// Pass the entire buffer into this process function
	// It will internally do frame-by-frame processing
	void process(float** ppfOutBuffer, int iNumChannels, int iNumFrames);

	float getInternalClockInSeconds() { return static_cast<float>(m_iSampleCounter.load() / m_fSampleRateInHz); };
	int64_t getInternalClockInSamples() { return m_iSampleCounter.load(); };

protected:

	void checkFlags() override;
	void checkTriggers() override;
	void checkQueues() override;

	// Will trash unused instruments every 500ms
	// Ensures shared_ptrs will not deallocate on audio thread
	void timerCallback() override;

	AtomicRingBuffer<std::shared_ptr<CInstrument>> m_RemoveQueue{ 1000 };
	AtomicRingBuffer<std::shared_ptr<CInstrument>> m_GarbageQueue{ 1000 };
};

#endif 
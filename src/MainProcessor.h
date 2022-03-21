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
	Error_t addInstRef(CInstrument& rInstToAdd);
	Error_t removeInstRef(CInstrument& rInstToRemove);
	Error_t addInst(CInstrument*& pInstToAdd);
	Error_t removeInst(CInstrument*& pInstToRemove);

	// Pass the entire buffer into this process function
	// It will internally do frame-by-frame processing
	void process(float** ppfOutBuffer, int iNumChannels, int iNumFrames);

	float getInternalClockInSeconds() { return static_cast<float>(m_iSampleCounter / m_fSampleRateInHz); };
	int64_t getInternalClockInSamples() { return m_iSampleCounter; };

protected:

	// Helper function to check maps for events like noteOn(), noteOff(), etc
	// This overrides from CScheduler, removing the key/value pair after being triggered
	virtual void checkTriggers() override;
	virtual void checkQueues() override;

	AtomicRingBuffer<CInstrument*> m_RemoveQueue{ 32 };
};

#endif 
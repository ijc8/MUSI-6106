#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

class MainProcessor : public Scheduler
{
public:
	MainProcessor() {};
	~virtual MainProcessor() {};

	void addInstRef(CInstrument& referenceToAdd, float duration, float onset);
	virtual void pushInst(CInstrument* instrumentToPush, float duration, float onset) override;
	void pushLoop(Looper* loopToPush, int numLoops);
	void pushSchedule(Scheduler* scheduleToPush)


protected:
private:

};

#endif 
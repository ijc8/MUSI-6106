#if !defined(__MainProcessor_hdr__)
#define __MainProcessor_hdr__

#include "Scheduler.h"

class MainProcessor : public Scheduler
{
public:
	MainProcessor() { noteOn(); };
	~MainProcessor() {};

	void addInstRef(CInstrument& instrumentToAdd);
	void removeInstRef(CInstrument& instrumentToRemove);

	void pushInst(CInstrument* instrumentToPush, float duration = 1.0f, float onset = 0.0f) override;

	void process(float** outBuffer, int numChannels, int numSamples, const int& masterClock);

protected:

	map<int, unordered_set<CInstrument*>> mapRemover;

};

#endif 
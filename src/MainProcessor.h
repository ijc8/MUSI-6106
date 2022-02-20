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

	void pushInst(CInstrument* instrumentToPush, float duration, float onset) override;

	float process() override;

protected:

	unordered_set<CInstrument*> checkTriggers(int currentSample, map<int, unordered_set<CInstrument*>>& mapToCheck) override;

	map<int, unordered_set<CInstrument*>> mapRemover;

};

#endif 
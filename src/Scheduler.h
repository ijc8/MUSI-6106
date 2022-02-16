#if !defined(__Scheduler_hdr__)
#define __Scheduler_hdr__

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "SoundProcessor.h"

class CScheduler : public CSoundProcessor
{
public:
	CScheduler(float fSampleRate = 0.0f);
	~CScheduler();

	float process() override;

	Error_t add(CInstrument* rInstrumentToAdd, float fOnsetInSec, float fDurationInSec);
	Error_t enableLoop(bool bShouldEnable);

private:

	std::unordered_map<int, std::unordered_set<CInstrument*>> m_ScheduleStarter;
	std::unordered_map<int, std::unordered_set<CInstrument*>> m_ScheduleEnder;
	std::vector<CInstrument*> m_InstrumentVector;

	int convertSecToSamp(float fSec) const;

	int iCurrentSample = 0;
};
#endif // #if !defined(__Scheduler_hdr__)
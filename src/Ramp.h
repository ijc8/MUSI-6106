#if !defined (_RAMP_H_)
#define _RAMP_H_

class Ramp
{
public:

	Ramp(float fSampleRate = 1.0f) : m_fSampleRate(fSampleRate) {};

	void setSampleRate(float fSampleRate) { m_fSampleRate = fSampleRate; };

	void rampTo(float fValue, float fTime)
	{
		m_fIncrement = (fValue - m_fCurrentValue) / (fTime * m_fSampleRate);
		m_iCounter = static_cast<int>(m_fSampleRate * fTime);
	}

	float process()
	{
		if (m_iCounter > 0)
		{
			m_iCounter--;
			m_fCurrentValue += m_fIncrement;
		}
		return m_fCurrentValue;
	}

	void setValue(float fValue)
	{
		m_fCurrentValue = fValue;
		m_iCounter = 0;
		m_fIncrement = 0.0f;
	}

	bool isFinished() const
	{
		return m_iCounter <= 0;
	}

private:

	float m_fCurrentValue = 0.0f;
	float m_fIncrement = 0.0f;
	float m_fSampleRate = 1.0f;
	int m_iCounter = 0;

};


#endif
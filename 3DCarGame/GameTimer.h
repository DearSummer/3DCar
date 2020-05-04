#pragma once
#include <basetsd.h>

class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;
	float DetlaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double m_SecondPerCount;
	double m_DetlaTime;

	INT64 m_BaseTime;
	INT64 m_PausedTime;
	INT64 m_StopTime;
	INT64 m_PrevTime;
	INT64 m_CurrTime;

	bool m_Stopped;
};

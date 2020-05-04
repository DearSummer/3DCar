#include "GameTimer.h"
#include <profileapi.h>


GameTimer::GameTimer()
	:m_SecondPerCount(0.0),
	m_DetlaTime(-1.0),
	m_BaseTime(0),
	m_PausedTime(0),
	m_StopTime(0),
	m_PrevTime(0),
	m_CurrTime(0),
	m_Stopped(false)
{
	INT64 countPerSecond;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countPerSecond));
	m_SecondPerCount = 1.0 / static_cast<double>(countPerSecond);
}


float GameTimer::DetlaTime() const
{
	return static_cast<float>(m_DetlaTime);
}


float GameTimer::TotalTime() const
{
	if(m_Stopped)
	{
		return static_cast<float>((m_StopTime - m_PausedTime - m_BaseTime) * m_SecondPerCount);
	}

	return static_cast<float>((m_CurrTime - m_PausedTime - m_BaseTime) * m_SecondPerCount);
}


void GameTimer::Reset()
{
	INT64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_PausedTime = 0;
	m_Stopped = false;
}

void GameTimer::Stop()
{
	if(!m_Stopped)
	{
		INT64 currTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

		m_StopTime = currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	if(m_Stopped)
	{
		m_DetlaTime = 0;
		return;
	}

	INT64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

	m_CurrTime = currTime;
	m_DetlaTime = (m_CurrTime - m_PrevTime) * m_SecondPerCount;
	m_PrevTime = m_DetlaTime;

	if (m_DetlaTime < 0.0)
		m_DetlaTime = 0;
}


void GameTimer::Start()
{
	INT64 startTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

	if (m_Stopped)
	{
		m_PausedTime += startTime - m_StopTime;

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}

}

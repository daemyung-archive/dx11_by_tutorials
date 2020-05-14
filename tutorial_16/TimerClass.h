#pragma once

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency = 0;
	float m_ticksPerMs = 0;
	INT64 m_startTime = 0;
	float m_frameTime = 0;
};
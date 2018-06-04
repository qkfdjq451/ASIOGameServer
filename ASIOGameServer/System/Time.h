#pragma once

class Time
{
public:
	void Update();
	static float Delta() { return inst ?  inst->delta : 0.0f; }

private:
	uint64_t frameCount;
	float delta;
	std::chrono::system_clock::time_point currentTime;
	std::chrono::system_clock::time_point lastTime;

private:
	Time();
	~Time();
	static Time* inst;
public:
	static bool Create();
	static bool	Delete();
	static Time* Get() { return inst; }

};
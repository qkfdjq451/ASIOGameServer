#include "../Global.h"
#include "Time.h"

Time::Time()
	:frameCount(0), delta(0)
{	
	lastTime = std::chrono::system_clock::now();
}

Time::~Time()
{

}

void Time::Update()
{
	currentTime = std::chrono::system_clock::now();
	std::chrono::duration<float> sec = currentTime - lastTime;
	delta = sec.count();

	++frameCount;
	lastTime = currentTime;
}


Time* Time::inst = nullptr;
bool Time::Create()
{
	if (inst == nullptr)
	{
		inst = new Time();
		return true;
	}
	return false;
}

bool Time::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

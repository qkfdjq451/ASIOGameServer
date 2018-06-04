#include "../Global.h"
#include "TimeManager.h"

using namespace std;
using namespace std::chrono;

TimerManager * TimerManager::inst = nullptr;
void TimerManager::Run()
{
	t = new std::thread([this]
	{
		while (true)
		{
			//현재 시간
			auto Now = system_clock::now();

			for (auto &loop : m_LoopList)
			{
				auto& timer = loop.second;
				//지나간 시간
				auto sec = duration_cast<milliseconds> (system_clock::now() - timer.start_clock);

				if (timer.unit_milliseconds < sec.count())
				{
					auto z = timer.start_clock - sec;
					timer.start_clock = system_clock::now();
					timer.work->func();
				}
			}

			queue<int> del;
			for (auto& time : m_TimerList)
			{
				auto& timer = time.second;
				auto sec = duration_cast<milliseconds> (system_clock::now() - timer.start_clock);

				if (timer.unit_milliseconds < sec.count())
				{
					//timer.start_clock = system_clock::now();
					timer.work->func();
					del.push(time.first);
				}
			}
			while (!del.empty())
			{
				m_TimerList.erase(del.front());
				del.pop();
			}
		}
	});
}

bool TimerManager::InsertLoop(int id, long milliseconds, shared_ptr<Func> func)
{
	Timer t;
	t.start_clock = system_clock::now();
	t.unit_milliseconds = milliseconds;
	t.work = func;
	auto result = m_LoopList.insert(make_pair(id, t));
	return result.second;
}

bool TimerManager::InsertTimer(int id, long milliseconds, shared_ptr<Func> func)
{
	Timer t;
	t.start_clock = system_clock::now();
	t.unit_milliseconds = milliseconds;
	t.work = func;
	auto result = m_TimerList.insert(make_pair(id, t));
	return result.second;
}

bool TimerManager::RemoveLoop(int id)
{
	auto result = m_LoopList.find(id);
	if (result != m_LoopList.end())
	{
		m_LoopList.erase(result);
		return true;
	}
	return false;
}

bool TimerManager::RemoveTimer(int id)
{
	auto result = m_TimerList.find(id);
	if (result != m_TimerList.end())
	{
		m_TimerList.erase(result);
		return true;
	}
	return false;
}

TimerManager::TimerManager()
{
}

TimerManager::~TimerManager()
{
}

bool TimerManager::Create()
{
	if (inst == nullptr)
	{
		inst = new TimerManager();
		return true;
	}
	return false;
}

bool TimerManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

TimerManager * TimerManager::Get()
{
	return inst;
}

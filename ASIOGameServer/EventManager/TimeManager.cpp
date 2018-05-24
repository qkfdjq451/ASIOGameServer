#include "../Global.h"
#include "TimeManager.h"

using namespace std;
using namespace std::chrono;

TimeManager * TimeManager::inst = nullptr;
void TimeManager::Run()
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

bool TimeManager::InsertLoop(int id, long milliseconds, shared_ptr<Func> func)
{
	Timer t;
	t.start_clock = system_clock::now();
	t.unit_milliseconds = milliseconds;
	t.work = func;
	auto result = m_LoopList.insert(make_pair(id, t));
	return result.second;
}

bool TimeManager::InsertTimer(int id, long milliseconds, shared_ptr<Func> func)
{
	Timer t;
	t.start_clock = system_clock::now();
	t.unit_milliseconds = milliseconds;
	t.work = func;
	auto result = m_TimerList.insert(make_pair(id, t));
	return result.second;
}

bool TimeManager::RemoveLoop(int id)
{
	auto result = m_LoopList.find(id);
	if (result != m_LoopList.end())
	{
		m_LoopList.erase(result);
		return true;
	}
	return false;
}

bool TimeManager::RemoveTimer(int id)
{
	auto result = m_TimerList.find(id);
	if (result != m_TimerList.end())
	{
		m_TimerList.erase(result);
		return true;
	}
	return false;
}

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

bool TimeManager::Create()
{
	if (inst == nullptr)
	{
		inst = new TimeManager();
		return true;
	}
	return false;
}

bool TimeManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

TimeManager * TimeManager::Get()
{
	return inst;
}

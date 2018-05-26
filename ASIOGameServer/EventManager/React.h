#pragma once
#include "../Global.h"

template<typename T>
class React
{
public:
	T Get() { return data; }
	void Set(const T& _data) 
	{ 		
		for (auto ev : prevEvents)
		{
			ev.second(_data);
		}
		data = _data; 
		for (auto ev : postEvents)
		{
			ev.second(_data);
		}
	}
	int InsertPrevEvent(std::function<void(T)> fun)
	{
		prevEvents.insert(std::make_pair(eventPrevCount, fun));
		return eventPrevCount++;
	}

	int InsertPostEvent(std::function<void(T)> fun)
	{
		postEvents.insert(std::make_pair(eventPostCount, fun));
		return eventPostCount++;
	}

	bool RemovePostEvent(int key)
	{
		auto result = postEvents.find(key);
		if (result != postEvents.end())
		{
			postEvents.erase(key);
			return true;
		}
		return false;
	}

	bool RemovePrevEvent(int key)
	{
		auto result = prevEvents.find(key);
		if (result != prevEvents.end())
		{
			prevEvents.erase(key);
			return true;
		}
		return false;
	}

	React() :  eventPrevCount(0) , eventPostCount(0)
	{}
private:
	T data;
	int eventPrevCount;
	int eventPostCount;
	std::map<int,std::function<void(T)>> prevEvents;
	std::map<int, std::function<void(T)>> postEvents;
};
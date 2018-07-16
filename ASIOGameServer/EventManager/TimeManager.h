#pragma once

struct STimer
{
	std::chrono::system_clock::time_point start_clock;
	long unit_milliseconds;
	std::shared_ptr<class Func> work;
};

class TimerManager
{
public:
	void Run();
	bool InsertLoop(int id, long milliseconds, std::shared_ptr<class Func> func);
	bool InsertTimer(int id, long milliseconds, std::shared_ptr<class Func> func);

	bool RemoveLoop(int id);
	bool RemoveTimer(int id);
	std::thread* GetThread() { return t; }
private:
	std::thread* t;
	TimerManager();
	~TimerManager();


	std::map<int, STimer> m_LoopList;
	std::map<int, STimer> m_TimerList;
	static TimerManager * inst;
public:
	static bool Create();
	static bool	Delete();
	static TimerManager* Get();
};

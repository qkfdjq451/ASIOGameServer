#pragma once

struct Timer
{
	std::chrono::system_clock::time_point start_clock;
	long unit_milliseconds;
	std::shared_ptr<class Func> work;
};

class TimeManager
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
	TimeManager();
	~TimeManager();


	std::map<int, Timer> m_LoopList;
	std::map<int, Timer> m_TimerList;
	static TimeManager * inst;
public:
	static bool Create();
	static bool	Delete();
	static TimeManager* Get();
};

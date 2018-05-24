#pragma once

class EventManager
{
public:
	bool InsertNewUserNotify(int id);
	bool RemoveNewUserNotify(int id);
private:
	void Call_NewUser(int a);

	std::set<int> m_NewUserNotify;


private:
	void NewUserPrint(int a, int id);

	EventManager();
	~EventManager();
	static EventManager * inst;
public:
	static bool Create();
	static bool	Delete();
	static EventManager* Get();
};

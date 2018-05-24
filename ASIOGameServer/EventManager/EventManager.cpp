#include "../Global.h"
#include "EventManager.h"

using namespace std;

EventManager * EventManager::inst = nullptr;

bool EventManager::Create()
{
	if (inst == nullptr)
	{
		inst = new EventManager();
		return true;
	}
	return false;
}

bool EventManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

EventManager * EventManager::Get()
{
	return inst;
}

bool EventManager::InsertNewUserNotify(int id)
{
	Call_NewUser(id);
	auto result = m_NewUserNotify.insert(id);
	return result.second;
}

bool EventManager::RemoveNewUserNotify(int id)
{
	auto result = m_NewUserNotify.find(id);
	if (result != m_NewUserNotify.end())
	{
		m_NewUserNotify.erase(result);
		return true;
	}
	return false;
}

void EventManager::Call_NewUser(int a)
{
	for (auto user : m_NewUserNotify)
	{
		NewUserPrint(a, user);
	}
}

void EventManager::NewUserPrint(int a, int id)
{
	using namespace std;
	cout << " 현재 ID : " << id << "에게 공지!" << endl;
	cout << "ID : " << a << "님이 접속하였습니다." << endl;
}

EventManager::EventManager()
{

}

EventManager::~EventManager()
{
}

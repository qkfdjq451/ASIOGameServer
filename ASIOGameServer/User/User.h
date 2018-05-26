#pragma once
#include "../Global.h"

class User :public std::enable_shared_from_this<User>
{
public:
	friend class UserManager;
	User();
	void Init();
	
public:	
	std::string GetName() { return nickname; }
	std::weak_ptr<class Session> GetSesstion() { return session; }
	void SetSession(std::shared_ptr<class Session> session);
	
	int GetMapKey() { return currentMapKey.Get(); }
private:

private:
	int key;

	std::string id;
	std::weak_ptr<class Session> session;
	
	
	//케릭터 정보
	std::string nickname;
	React<int> currentMapKey;

};

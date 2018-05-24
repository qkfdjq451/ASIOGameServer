#pragma once
#include "../Global.h"

class User :public std::enable_shared_from_this<User>
{
public:
	friend class UserManager;
	User();


public:	
	std::string GetName() { return nickname; }
	std::weak_ptr<class Session> GetSesstion() { return session; }
	void SetSession(std::shared_ptr<class Session> session);

	
private:

private:
	int key;

	std::string id;
	std::string nickname;

	std::weak_ptr<class Session> session;
};

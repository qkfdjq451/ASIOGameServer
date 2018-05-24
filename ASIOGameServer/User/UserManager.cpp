#include "UserManager.h"
#include "User.h"


std::shared_ptr<class User> UserManager::CreateUser(int key)
{
	auto result = users.find(key);
	//중복된 유저가 없을 시 생성
	if (result == users.end())
	{
		auto user = std::make_shared<User>();
		user->key = key;
		user->id = std::to_string(key);
		user->nickname = std::to_string(key);
		users.insert(std::make_pair(key, user));
		return user;
	}
	return nullptr;
}



UserManager::UserManager()
{

}

UserManager::~UserManager()
{

}


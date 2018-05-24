#pragma once

#include "../Global.h"

class UserManager : public Component
{
public:
	//Post 함수
	std::shared_ptr<class User> CreateUser(int key);
	
private:
	std::unordered_map<int, std::shared_ptr<class User>> users;

	//생성자
public:
	UserManager();
	~UserManager();
	

};
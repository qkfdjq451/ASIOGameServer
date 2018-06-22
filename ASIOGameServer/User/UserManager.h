#pragma once

#include "../Global.h"

class UserManager : public Component 
{
public:
	//Post 함수
	std::shared_ptr<class User> CreateUser(int key);	
	void SendAllUsers(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	

	//전 UserManager에서 User 삭제
	bool RemoveUser(int key);
	void LoopFunction(std::function<void(std::shared_ptr<class User>)> func);

private:
	std::unordered_map<int, std::shared_ptr<class User>> users;
	
	//생성자
public:
	UserManager();
	~UserManager();
};
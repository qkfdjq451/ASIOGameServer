#pragma once

#include "../Global.h"

class UserManager : public Component 
{
public:
	//Post �Լ�
	std::shared_ptr<class User> CreateUser(int key);	
	void SendAllUsers(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	

	//�� UserManager���� User ����
	bool RemoveUser(int key);
	void LoopFunction(std::function<void(std::shared_ptr<class User>)> func);

private:
	std::unordered_map<int, std::shared_ptr<class User>> users;
	
	//������
public:
	UserManager();
	~UserManager();
};
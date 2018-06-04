#include "UserManager.h"
#include "User.h"
#include "../Asio/Session/Session.h"

#include "../Map/MapInfo.h"

std::shared_ptr<class User> UserManager::CreateUser(int key)
{
	auto result = users.find(key);
	//중복된 유저가 없을 시 생성
	if (result == users.end())
	{
		auto user = std::make_shared<User>();
		
		//TODO : 나중에 DB에서 값을 가져와서 받게함
		user->key = key;
		user->id = std::to_string(key);
		users.insert(std::make_pair(key, user));

		//TODO : Character 생성후 CharacterManager에 넣게 하기


		return user;
	}
	return nullptr;
}

void UserManager::SendAllUsers(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
{
	auto sb = std::make_shared<SendBuffer>(symbol, move(fbb));
	for (auto user : users)
	{
		auto session = user.second->GetSesstion().lock();
		if (session != nullptr)
		{
			session->PushSend(sb);
		}
	}
}

bool UserManager::RemoveUser(int key)
{	
	auto result = users.find(key);
	if (result != users.end())
	{
		auto user = result->second;
		auto  MainUserFunc=std::make_shared<Function<int>>([](int key)
		{
			auto um = WorkerGroup::Get_IOGroup()->GetComponent<UserManager>();
			um->users.erase(key);
		},key);
		WorkerGroup::Get_IOGroup()->PostFuction(move(MainUserFunc));

		return true;
	}

	return false;
}

void UserManager::LoopFunction(std::function<void(std::shared_ptr<class User>)> func)
{
	for (auto user : users)
	{
		func(user.second);
	}
}

UserManager::UserManager()
{

}

UserManager::~UserManager()
{

}


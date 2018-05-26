#include "UserManager.h"
#include "User.h"
#include "../Asio/Session/Session.h"
#include "../Map/MapManager.h"
#include "../Map/MapInfo.h"

std::shared_ptr<class User> UserManager::CreateUser(int key)
{
	auto result = users.find(key);
	//중복된 유저가 없을 시 생성
	if (result == users.end())
	{
		auto user = std::make_shared<User>();
		user->Init();
		//TODO : 나중에 DB에서 값을 가져와서 받게함

		user->key = key;
		user->id = std::to_string(key);
		user->nickname = std::to_string(key);		
		user->currentMapKey.Set(0);

		users.insert(std::make_pair(key, user));
/*
		auto  MapUserFunc = std::make_shared<Function<int>>([user](int key)
		{
			auto mm = WorkerGruop::Get_InGameGroup()->GetRoot()->GetComponent<MapManager>();
			auto mapinfo = mm->Get_MapInfo(user->currentMapKey);
			auto um = mapinfo->GetComponent<UserManager>();
			um->InsertUser(user);
		}, key);
		WorkerGruop::Get_InGameGroup()->PostFuction(move(MapUserFunc));
*/
		return user;
	}
	return nullptr;
}

bool UserManager::InsertUser(std::shared_ptr<class User> user)
{
	auto result = users.find(user->key);
	if (result == users.end()) 
	{
		users.insert(std::make_pair(user->key, user));
	}
	return false;
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
			auto um = WorkerGruop::Get_IOGroup()->GetRoot()->GetComponent<UserManager>();
			um->users.erase(key);
		},key);

		auto  MapUserFunc = std::make_shared<Function<int>>([user](int key)
		{
			auto mm = WorkerGruop::Get_InGameGroup()->GetRoot()->GetComponent<MapManager>();
			auto mapinfo = mm->Get_MapInfo(key);
			auto um = mapinfo->GetComponent<UserManager>();
			um->users.erase(key);
		},key);
		WorkerGruop::Get_IOGroup()->PostFuction(move(MainUserFunc));
		WorkerGruop::Get_InGameGroup()->PostFuction(move(MapUserFunc));

		return true;
	}

	return false;
}

UserManager::UserManager()
{

}

UserManager::~UserManager()
{

}


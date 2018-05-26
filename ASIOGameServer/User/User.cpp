
#include "User.h"
#include "UserManager.h"

#include "../Asio/Session/Session.h"
#include "../Map/MapManager.h"
#include "../Map/MapInfo.h"


User::User()
{
	

}

void User::Init()
{
	auto self = shared_from_this();

	currentMapKey.InsertPrevEvent([](int key)
	{
		auto func = std::make_shared < Function<int>>([](int key)
		{
			auto mm = WorkerGruop::Get_InGameGroup()->GetRoot()->GetComponent<MapManager>();
			auto mapinfo = mm->Get_MapInfo(key);
			if (mapinfo)
			{
				auto um = mapinfo->GetComponent<UserManager>();
				um->RemoveUser(key);
			}
		}, key);
		WorkerGruop::Get_InGameGroup()->PostFuction(move(func));
	});

	currentMapKey.InsertPostEvent([self](int key)
	{
		auto func = std::make_shared < Function<int>>([self](int key)
		{
			auto mm = WorkerGruop::Get_InGameGroup()->GetRoot()->GetComponent<MapManager>();
			auto mapinfo = mm->Get_MapInfo(key);
			if (mapinfo)
			{
				auto um = mapinfo->GetComponent<UserManager>();
				um->InsertUser(self);
			}
		}, key);
		WorkerGruop::Get_InGameGroup()->PostFuction(move(func));
	});
}

void User::SetSession(std::shared_ptr<class Session> _session)
{
	session = _session;
}

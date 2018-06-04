#include "../Global.h"
#include "Character.h"

#include "../Map/MapInfo.h"
#include "../Map/MapManager.h"
#include "../Channel/Channel.h"

Character::Character(std::shared_ptr<class User> user)
	:user(user)
{	
}

Character::~Character()
{
	mapKey.Set(0);
}

void Character::Init()
{
	auto self = shared_from_this();
	mapKey.Set(0);

	mapKey.InsertPrevEvent([self,this](int mapKey)
	{		
		channel.Set(0);
	});

	mapKey.InsertPostEvent([this,self](int mapKey)
	{
		vector<std::shared_ptr<Component>> vec;
		Component::GetComponents_For_Tag("MapManager", vec);
		auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
		if (mm)
		{
			auto mapinfo = mm->GetMapInfo(mapKey);
			mapinfo->InsertCharacter(self);
		}

	});

	channel.InsertPrevEvent([this](int channel)
	{
		if (channel != 0)
		{
			//TODO : 채널이 변경됐을 때 이전에 해야할 처리
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (mm)
			{
				auto mapinfo = mm->GetMapInfo(mapKey.Get());
				mapinfo->GetChannel(channel)->Async_EraseCharacter(code);
			}
		}
	});

	channel.InsertPostEvent([self, this](int key)
	{
		//TODO : 채널이 변경된 이후에 해야할 처리
		if (key != 0)
		{
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (mm)
			{
				auto mapinfo = mm->GetMapInfo(mapKey.Get());
				mapinfo->GetChannel(key)->Async_InsertCharacter(self);
			}
		}

	});
}


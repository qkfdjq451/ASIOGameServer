#include "../Global.h"
#include "MapInfo.h"
#include "../Channel/Channel.h"
#include "../User/UserManager.h"
#include "../Character/CharacterManager.h"
#include "../Character/Character.h"

MapInfo::MapInfo()
{

}

void MapInfo::BeginPlay()
{
	for (int i = 1; i <= 3; i++)
	{
		auto channel = Component::CreateComponent<Channel>();
		channel->SetNumber(i);
		Attach(channel);
		channels.push_back(channel);
	}
}

void MapInfo::PrevTick()
{

}

void MapInfo::Tick()
{
	PrevTick();

}

void MapInfo::EndPlay()
{

}

bool MapInfo::InsertCharacter(std::shared_ptr<class Character> character)
{
	vector<shared_ptr<Channel>> vec;
	GetComponents<Channel>(vec);

	for (auto ch : vec)
	{
		auto cm = ch->GetComponent<CharacterManager>();
		if (cm)
		{
			if (cm->GetCharacterCount() < Channel::Max_Character_Count)
			{				
				character->SetChannel(ch->GetNumber());
				return true;
			}
		}		
	}
	return false;
}

std::shared_ptr<class Channel> MapInfo::GetChannel(int index)
{
	if (channels.size() < index || index < 0)
		return nullptr;
	return channels[index - 1].lock();
}

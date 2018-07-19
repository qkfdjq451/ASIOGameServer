#include "../Global.h"
#include "Channel.h"
#include "../Character/CharacterManager.h"
#include "../Character/Character.h"
#include "../User/User.h"
#include "../Asio/Session/Session.h"
#include "../Map/MapInfo.h"
#include "../Map/MapManager.h"
#include "../Monster/MonsterManager.h"

Channel::Channel(int mapcode, int channelNumber)
	:mapCode(mapcode),number(channelNumber)
{

}

void Channel::Async_InsertCharacter(std::shared_ptr<class Character> character)
{
	auto self = shared_from_this();
	async(std::launch::async, [self,this, character]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_Insert_list.push_back(character);
	});
}

void Channel::Async_EraseCharacter(int key)
{
	auto self = shared_from_this();
	async(std::launch::async, [self,this, key]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_erase_list.push_back(key);
		
	});
}


void Channel::BeginPlay()
{	
	auto cm=Component::CreateComponent<CharacterManager>();
	if (cm)
	{
		cm->SetMovable(true);
		Attach(cm);
	}	
	auto mapInfo = GetParentComponent<MapInfo>();	
	if (mapInfo!=nullptr )
	{
		if (MapType::Dungeon == mapInfo->GetMapType())
		{
			auto monstermanager = Component::CreateComponent<MonsterManager>();
			if (monstermanager)
			{
				auto descs = mapInfo->GetSpawnDesc();
				for (auto desc : descs)
				{
					monstermanager->Add_Spawn_Monster(desc);
				}
				Attach(monstermanager);
			}	
		}
	}
}

void Channel::PrevTick()
{
	std::vector<int> copy_erase_list;
	std::vector<std::shared_ptr<class Character>> copy_Insert_list;

	{
		std::lock_guard<std::mutex> lock(mt);
		copy_erase_list.assign(req_erase_list.begin(), req_erase_list.end());
		copy_Insert_list.assign(req_Insert_list.begin(), req_Insert_list.end());
		req_erase_list.clear();
		req_Insert_list.clear();
	}

	for (auto erase : copy_erase_list)
	{
		EraseCharacter(erase);
	}

	for (auto insert : copy_Insert_list)
	{
		InsertCharacter(insert);
	}

}

void Channel::Tick()
{
	PrevTick();
	
}

void Channel::EndPlay()
{

}

bool Channel::InsertCharacter(std::shared_ptr<class Character> character)
{
	character->bMove = false;
	character->bChangePosition = false;
	character->SetWarpPosition();

	auto cm = Component::GetComponent<CharacterManager>();
	bool result = cm->InsertCharacter(character);
	if(result)
	{
		auto user = character->GetUserPointer().lock();
		auto session = user->GetSesstion().lock();

		auto mapInfo = GetParentComponent<MapInfo>();
		if (mapInfo != nullptr)
		{
			if (MapType::Dungeon == mapInfo->GetMapType())
			{
				auto monsterMng = GetComponent<MonsterManager>();
				if (monsterMng)
				{
					vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>> fbbArray;
					int total = monsterMng->Make_FBB_All_Monster(fbbArray);
					if (total != 0)
					{
						for (auto fbb3 : fbbArray)
						{
							auto monsterVec = FB::GetMonsterVec(fbb3->GetBufferPointer());
							auto mv = monsterVec->monvector();
							session->PushSend(PS::RESPAWN_MONSTER_VEC, fbb3);
						}
					}
				}
			}
		}
		vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>> fbbArray;
		auto total = cm->Make_FBB_All_CharacterInfo(fbbArray);
		if (total != 0)
		{
			for (auto fbb2 : fbbArray)
			{
				session->PushSend(PS::ENTER_NEW_CHARACTER_VECTOR, move(fbb2));
			}
		}
		
		
		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
		auto nick = fbb->CreateString(character->GetName());
		auto charB = FB::CharacterBuilder(*fbb);
		charB.add_code(character->GetCode());
		charB.add_type(character->GetTypeCode());
		charB.add_hp(character->GetMaxHP());
		charB.add_level(character->GetLevel());
		charB.add_nick(nick);
		charB.add_power(character->GetPower());
		charB.add_speed(character->GetSpeed());		
		charB.add_position(&character->GetPosition().ToFBVector3());
		auto charoffset=charB.Finish();
		fbb->Finish(charoffset);
		cm->SendAllCharacter(PS::ENTER_NEW_CHARACTER, move(fbb));
		printf("케릭터 정보 보내기!!\n");

		return true;
	}

	return false;
}

bool Channel::EraseCharacter(int key)
{
	auto cm = Component::GetComponent<CharacterManager>();
	bool result = cm->EraseCharacter(key);
	if (result)
	{
		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
		auto charB = FB::CharacterBuilder(*fbb);
		charB.add_code(key);
		auto charoffset = charB.Finish();
		fbb->Finish(charoffset);
		cm->SendAllCharacter(PS::LEAVE_CHARACTER, move(fbb));
		return true;
	}	
	return false;
}


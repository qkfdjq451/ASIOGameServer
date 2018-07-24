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

void Channel::Async_Func(function<void()> func)
{
	auto self = shared_from_this();
	async(std::launch::async, [self, this, func]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_func_list.push_back(func);
	});
}


void Channel::Escape(shared_ptr<Character> character)
{
	auto mapinfo = GetParentComponent<MapInfo>();
	if (!mapinfo) return;
	character->SetPosition(mapinfo->GetEscapeCoordinates());
	auto cm = character->GetCharacterManager().lock();
	auto fbb = make_shared<flatbuffers::FlatBufferBuilder>();
	auto moveB = FB::MoveBuilder(*fbb);
	moveB.add_code(character->GetCode());
	moveB.add_position(&character->GetPosition().ToFBVector3());
	fbb->Finish(moveB.Finish());
	cm->Async_SendAllCharacter(PS::CON_ESCAPE, move(fbb));
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
	std::vector<function<void()>> copy_func_list;
	{
		std::lock_guard<std::mutex> lock(mt);
		copy_erase_list.assign(req_erase_list.begin(), req_erase_list.end());
		copy_Insert_list.assign(req_Insert_list.begin(), req_Insert_list.end());
		copy_func_list.assign(req_func_list.begin(), req_func_list.end());
		req_erase_list.clear();
		req_Insert_list.clear();
		req_func_list.clear();
	}

	for (auto erase : copy_erase_list)
	{
		EraseCharacter(erase);
	}

	for (auto insert : copy_Insert_list)
	{
		InsertCharacter(insert);
	}
	for (auto func : copy_func_list)
	{
		func();
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
	character->SetMoveCancel();
	character->SetWarpPosition();

	auto cm = Component::GetComponent<CharacterManager>();
	if (!cm)
	{
		printf("실패 \n");
	}
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

		auto fbb = character->Make_Fbb_Character();
		cm->SendAllCharacter(PS::ENTER_NEW_CHARACTER, move(fbb));
		printf("케릭터 정보 보내기!!\n");

		return true;
	}

	return false;
}

bool Channel::EraseCharacter(int key)
{
	auto cm = Component::GetComponent<CharacterManager>();
	auto character = cm->GetCharacter(key);
	
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


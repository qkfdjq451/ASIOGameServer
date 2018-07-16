#include "../Global.h"

#include "MonsterManager.h"
#include "Monster.h"

#include "../Asio/Session/Session.h"
#include "../Map/MapInfo.h"
#include "../User/User.h"
#include "../Monster/MonsterBuilder.h"
#include "../Channel/Channel.h"

#include "../Character/Character.h"
#include "../Character/CharacterManager.h"

//std::shared_ptr<flatbuffers::FlatBufferBuilder> CharacterManager::Make_FBB_All_CharacterInfo()
//{
//	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
//	vector<flatbuffers::Offset<FB::Character>> vec;
//	for (auto character : characters)
//	{
//		auto ch = character.second.lock();
//		if (ch)
//		{
//			auto nick = fbb->CreateString(ch->GetName());
//			auto charB = FB::CharacterBuilder(*fbb);
//			charB.add_code(ch->GetCode());
//			charB.add_hp(ch->GetHP());
//			charB.add_level(ch->GetLevel());
//			charB.add_nick(nick);
//			charB.add_power(ch->GetPower());
//			charB.add_speed(ch->GetSpeed());
//			charB.add_position(&ch->GetPosition().ToFBVector3());
//			vec.push_back(charB.Finish());
//		}
//	}
//	auto vecoffset = fbb->CreateVector(vec);
//	fbb->Finish(FB::CreateCharacterVec(*fbb, vecoffset));
//	return fbb;
//}

void MonsterManager::Async_Function(std::shared_ptr<Func> func)
{
	async(std::launch::async, [this, func]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_func_list.push_back(func);
	});
}

void MonsterManager::DamageProcess(shared_ptr<char[]> data, shared_ptr<class Character> character)
{
	auto damageTable = FB::GetDamageVec(data.get());
	auto damageVecter = damageTable->damagevector();
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);	
	vector<flatbuffers::Offset<FB::Damage>> damageVector;	

	for (auto damage : *damageVecter)
	{		
		auto result = monsters.find(damage->damaged_code());
		if (result == monsters.end()) continue;
		auto monster = result->second.lock();
		if (!monster) continue;	
		bool retval = monster->CheckRange(character, damage->attackType());
		if (retval == false)
		{
			cout << "사거리 XXXXXXX" << endl;
			continue;
		}
		FB::DamageBuilder damageB = FB::DamageBuilder(*fbb);
		retval = monster->GetDamage(character, damage->attackType(), &damageB);
		if (retval)
			damageVector.push_back(damageB.Finish());
		else
			damageB.Finish();
		
	}
	if (damageVector.size() == 0) 
		return;

	auto offset = fbb->CreateVector(damageVector);
	fbb->Finish(FB::CreateDamageVec(*fbb, offset));
	auto channel = GetParentComponent<Channel>();
	if (!channel) return;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return;
	cm->Async_SendAllCharacter(PS::CON_DAMAGE_VECTOR, fbb);
}

void MonsterManager::BeginPlay()
{
	
}

void MonsterManager::PrevTick()
{
	std::vector<std::shared_ptr<Func>>  copy_func_list;
	{
		std::lock_guard<std::mutex> lock(mt);
		copy_func_list.assign(req_func_list.begin(), req_func_list.end());
		req_func_list.clear();
	}

	for (auto func : copy_func_list)
	{
		func->func();
	}
	copy_func_list.clear();
}

void MonsterManager::Tick()
{
	PrevTick();
	saveTime += Time::Delta();
	if (saveTime > 0.4)
	{
		saveTime = 0;
		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
		vector<flatbuffers::Offset<FB::Move>> moveVec;
		for (auto monster : monsters)
		{
			auto mon = monster.second.lock();
			mon->GetMoveInfo(fbb, moveVec);
		}

		if (moveVec.size() > 0)
		{
			auto vec = fbb->CreateVector(moveVec);
			auto movebb = FB::CreateMoveVec(*fbb, vec);
			fbb->Finish(movebb);

			auto channel = GetParentComponent<Channel>();
			if (!channel) return;
			auto cm = channel->GetComponent<CharacterManager>();
			if (!cm) return;
			cm->Async_SendAllCharacter(PS::MONSTER_MOVING_VECTOR, fbb);
		}
	}
}

void MonsterManager::EndPlay()
{
	
}

std::shared_ptr<flatbuffers::FlatBufferBuilder> MonsterManager::Make_FBB_All_Monster()
{
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
	vector<flatbuffers::Offset<FB::Monster>> vec;
	for (auto node : monsters)
	{
		auto monster = node.second.lock();
		if (monster)
		{
			auto nick = fbb->CreateString(monster->GetName());
			auto monsterB = FB::MonsterBuilder(*fbb);
			monsterB.add_code(monster->GetCode());
			monsterB.add_number(monster->GetNumber());
			monsterB.add_MaxHp(monster->GetMaxHP());
			monsterB.add_CurrentHP(monster->GetCurrentHp());
			monsterB.add_position(&monster->GetPosition().ToFBVector3());
			monsterB.add_nick(nick);
			monsterB.add_power(monster->GetPower());
			monsterB.add_speed(monster->GetSpeed());
			vec.push_back(monsterB.Finish());
		}
	}
	printf("FBB에 저장한 몬스터의 수 : %d", vec.size());
	auto vecoffset = fbb->CreateVector(vec);
	fbb->Finish(FB::CreateMonsterVec(*fbb, vecoffset));

	return fbb;
}

void MonsterManager::Add_Spawn_Monster(const MonsterSpawnDesc & desc)
{
	for (int i = 0; i < desc.monsterCount; i++)
	{
		auto monster = MonsterBuilder::CreateMonster(desc.monsterCode, monKeyCount++);
		if (monster)
		{
			monster->navi = desc.navi;
			monster->respawnPosition = desc.position;
			monster->respawnRange = desc.respawnTime;

			monster->respawnTime = desc.respawnTime;
			monsters.insert(make_pair(monster->GetNumber(), monster));
			Attach(monster);
		}
	}
	
}

MonsterManager::MonsterManager()
{
	bMovable = false;
	saveTime = 0;
	monKeyCount = 0;
}

MonsterManager::~MonsterManager()
{

}


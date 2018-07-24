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

void MonsterManager::DamageProcessVector(shared_ptr<char[]> data, shared_ptr<class Character> character)
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
			printf("사거리 XXXXXXX\n");
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

void MonsterManager::DamageProcess(shared_ptr<char[]> data, shared_ptr<class Character> character)
{
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
	auto damage = FB::GetDamage(data.get());
	auto result = monsters.find(damage->damaged_code());
	if (result == monsters.end()) return;
	auto monster = result->second.lock();
	if (!monster) return;
	bool retval = monster->CheckRange(character, damage->attackType());
	if (retval == false)
	{
		printf("사거리 XXXXXXX\n");
		return;
	}
	FB::DamageBuilder damageB = FB::DamageBuilder(*fbb);
	retval = monster->GetDamage(character, damage->attackType(), &damageB);
	if (retval)
	{
		fbb->Finish(damageB.Finish());
		auto channel = GetParentComponent<Channel>();
		if (!channel) return;
		auto cm = channel->GetComponent<CharacterManager>();
		if (!cm) return;
		cm->Async_SendAllCharacter(PS::CON_DAMAGE, fbb);
	}
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
		vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>> fbbArray;
		vector<std::shared_ptr<vector<flatbuffers::Offset<FB::Move>>>> moveVecArray;
		std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
		std::shared_ptr<vector<flatbuffers::Offset<FB::Move>>> moveVec= std::make_shared<vector<flatbuffers::Offset<FB::Move>>>();
		int count = 0;
		int total = 0;

		for (auto monster : monsters)
		{
			auto mon = monster.second.lock();
			mon->GetMoveInfo(fbb, *moveVec);
			count++;
			if (count == 10)
			{
				fbbArray.push_back(fbb);
				moveVecArray.push_back(moveVec);
				fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
				moveVec = std::make_shared<vector<flatbuffers::Offset<FB::Move>>>();
				count = 0;
			}
			total++;
		}

		if (count != 0)
		{
			fbbArray.push_back(fbb);
			moveVecArray.push_back(moveVec);
		}


		if (total > 0)
		{
			auto channel = GetParentComponent<Channel>();
			if (!channel) return;
			auto cm = channel->GetComponent<CharacterManager>();
			if (!cm) return;

			for (int i = 0; i < fbbArray.size(); i++)
			{
				fbb = fbbArray[i];
				moveVec = moveVecArray[i];
				auto vec = fbb->CreateVector(*moveVec);
				auto movebb = FB::CreateMoveVec(*fbb, vec);
				fbb->Finish(movebb);
				cm->Async_SendAllCharacter(PS::MONSTER_MOVING_VECTOR, fbb);
			}
		}
	}
}

void MonsterManager::EndPlay()
{
	
}

int MonsterManager::Make_FBB_All_Monster(vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>>&fbbArray, int slice)
{
	vector<std::shared_ptr<vector<flatbuffers::Offset<FB::Monster>>>> monsterVecArray;

	std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
	std::shared_ptr<vector<flatbuffers::Offset<FB::Monster>>> monsterVec = std::make_shared<vector<flatbuffers::Offset<FB::Monster>>>();

	int count = 0;
	int total = 0;

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
			monsterVec->push_back(monsterB.Finish());
			count++;

			if (count == slice)
			{
				count = 0;
				fbbArray.push_back(fbb);
				monsterVecArray.push_back(monsterVec);
				auto vecoffset = fbb->CreateVector(*monsterVec);
				fbb->Finish(FB::CreateMonsterVec(*fbb, vecoffset));
				fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
				monsterVec = std::make_shared<vector<flatbuffers::Offset<FB::Monster>>>();
			}
			total++;

			if (count != 0)
			{
				fbbArray.push_back(fbb);
				monsterVecArray.push_back(monsterVec);
				auto vecoffset = fbb->CreateVector(*monsterVec);
				fbb->Finish(FB::CreateMonsterVec(*fbb, vecoffset));
			}
		}
	}
	printf("FBB에 저장한 몬스터의 수 : %d\n", total);
	
	return total;
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


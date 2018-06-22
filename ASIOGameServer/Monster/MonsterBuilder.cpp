#include "../Global.h"
#include "MonsterBuilder.h"
#include "Monster.h"
#include "../BehaviorTree/BehaviorTree.h"

std::map<int, MonsterInfo> MonsterBuilder::infos;

void MonsterBuilder::Init()
{
	MySQLManager::Get()->Async_Query("select * from MonsterInfo",
		[](QVector& vec, bool result)
	{
		//정상적으로 받았을 때
		if (result)
		{
			for (auto info : vec)
			{
				MonsterInfo monster;
				monster.code = stoi(info[0].second);
				monster.name = (info[1].second);
				monster.HP = (float)stof(info[2].second);
				monster.Power = (float)stof(info[3].second);
				infos.insert(make_pair(monster.code, monster));
			}
		}
		else
		{
			//예외처리
		}
	});
}

std::shared_ptr<class Monster> MonsterBuilder::CreateMonster(int monstercode,int number)
{
	auto iter = infos.find(monstercode);
	if (iter != infos.end())
	{
		auto info = iter->second;
		auto monster = Component::CreateComponent<Monster>(monstercode);
		if (monster)
		{
			monster->SetNumber(number);
			monster->SetName(info.name);
			monster->SetMaxHP(info.HP);
			monster->SetPower(info.Power);
			monster->respawnTime = 10.0f;
			monster->speed = 400.0f;
			monster->ai = InsertAI(monster);
		}
		return monster;
	}
	return nullptr;
}

BTNode* MonsterBuilder::InsertAI(std::shared_ptr<class Monster> monster)
{
	BTNode* node = nullptr;
	if (monster->code == 1)
	{
		node= monster->ai =BehaviorTreeBuilder::Create()
		.sequence()
			.actor(std::bind(&Monster::UpdateState, monster, std::placeholders::_1))
			.selector((int*)&(monster->state))
				.sequence(Monster::State::IDLE)
					.actor(std::bind(&Monster::UpdateIdle, monster, std::placeholders::_1))
					.selector((int*)&monster->idleState)
						.actor(Monster::IdleState::PATROL, std::bind(&Monster::Patrol, monster,std::placeholders::_1))
						.actor(Monster::IdleState::HOLD, std::bind(&Monster::Hold, monster, std::placeholders::_1))
					.end()
				.end()
				.sequence(Monster::State::CHASE)
					.actor(std::bind(&Monster::Chase, monster,std::placeholders::_1))
				.end()
				.sequence(Monster::State::ATTACK)
					.actor(std::bind(&Monster::Attack, monster, std::placeholders::_1))
				.end()				
			.end()
		.end()
		.Finish();
	}		
	return node;
}
#include "../Global.h"
#include "MonsterBuilder.h"
#include "Monster.h"

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
		}
		return monster;
	}
	return nullptr;
}

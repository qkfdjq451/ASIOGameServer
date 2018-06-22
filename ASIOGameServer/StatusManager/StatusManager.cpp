#include "../Global.h"
#include "StatusManager.h"
#include "../Character/Character.h"

StatusManager* StatusManager::inst = nullptr;

bool StatusManager::SetStatus(std::shared_ptr<class Character> character)
{
	auto result = statusInfos.find(character->GetTypeCode());
	if (result == statusInfos.end()) return false;
	int level = character->GetLevel();
	character->SetMaxHP(level*result->second.growthHP + result->second.baseHP);
	character->SetDefense(level*result->second.growthDefense + result->second.baseDefense);
	character->SetPower(level*result->second.growthPower + result->second.basePower);
	character->SetSpeed(result->second.baseSpeed);
	return true;
}

StatusManager::StatusManager()
{
	string query = "select * from DamageFormula";
	MySQLManager::Get()->Async_Query(query.c_str(),
		[this](QVector& vec, bool result)
	{
		//정상적으로 받았을 때
		if (result)
		{
			for (auto datas : vec)
			{
				StatusInfo status;
				float* temp = (float*)&status;
				for (int i = 2; i < datas.size(); i++)
				{
					temp[i - 2] = stof(datas[i].second);
				}
				statusInfos.insert(make_pair(stoi(datas[0].second), status));
			}
		}
		else
		{
			//예외처리
		}
	});
}

StatusManager::~StatusManager()
{

}

bool StatusManager::Create()
{
	if (inst == nullptr)
	{
		inst = new StatusManager();
		return true;
	}
	return false;
}

bool StatusManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

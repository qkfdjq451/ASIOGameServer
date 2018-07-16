#include "../Global.h"
#include "DamageFormula.h"

DamageFormula* DamageFormula::inst = nullptr;

bool DamageFormula::GetDamage(int code, Damage & damage)
{
	auto result = damages.find(code);
	if (result == damages.end()) return false;
	damage = result->second;
	return true;
}

bool DamageFormula::GetCurDamage(int code, FB::AttackState state, float level, float power, float& cur, float& formula)
{
	auto result = damages.find(code);
	if (result == damages.end()) return false;
	switch (state)
	{
	case FB::AttackState_Combo1:
		formula = result->second.combo1 + result->second.growCombo1*level;
		break;
	case FB::AttackState_Combo2:
		formula = result->second.combo2 + result->second.growCombo2*level;
		break;
	case FB::AttackState_Combo3:
		formula = result->second.combo3 + result->second.growCombo3*level;
		break;
	case FB::AttackState_Skill1:
		formula = result->second.skill1 + result->second.growSkill1*level;
		break;
	case FB::AttackState_Skill2:
		formula = result->second.skill2 + result->second.growSkill2*level;
		break;
	case FB::AttackState_Skill3:
		formula = result->second.skill3 + result->second.growSkill3*level;
		break;
	}
	cur = formula * power;
	return true;
}

bool DamageFormula::CheckRange(int attackerCode, FB::AttackState state, Vector3 from, Vector3 to)
{
	auto result = ranges.find(attackerCode);
	if (result == ranges.end()) return false;
	float range;
	switch (state)
	{
	case FB::AttackState_Combo1:
		range = result->second.combo1;
		break;
	case FB::AttackState_Combo2:
		range = result->second.combo2;
		break;
	case FB::AttackState_Combo3:
		range = result->second.combo3;
		break;
	case FB::AttackState_Skill1:
		range = result->second.skill1;
		break;
	case FB::AttackState_Skill2:
		range = result->second.skill2;
		break;
	case FB::AttackState_Skill3:
		range = result->second.skill3;
		break;
	default:
		return false;
	}
	float distance = Vector3::Distance(from, to);
	cout << "플레이어와의 거리 : " << distance << endl;
	if (distance < range)
		return true;
	return false;
}

DamageFormula::DamageFormula()
{
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
					Damage damage;
					float* temp = (float*)&damage;
					for (int i = 1; i < datas.size(); i++)
					{
						temp[i - 1] = stof(datas[i].second);
					}
					damages.insert(make_pair(stoi(datas[0].second), damage));
				}
			}
			else
			{
				//예외처리
			}
		});
	}
	{
		string query = "select * from CharacterAttackRange";
		MySQLManager::Get()->Async_Query(query.c_str(),
			[this](QVector& vec, bool result)
		{
			//정상적으로 받았을 때
			if (result)
			{
				for (auto datas : vec)
				{
					Range range;
					float* temp = (float*)&range;
					for (int i = 1; i < datas.size(); i++)
					{
						temp[i - 1] = stof(datas[i].second);
					}
					ranges.insert(make_pair(stoi(datas[0].second), range));
				}
			}
			else
			{
				//예외처리
			}
		});
	}
}
DamageFormula::~DamageFormula()
{
}
bool DamageFormula::Create()
{
	if (inst == nullptr)
	{
		inst = new DamageFormula();
		return true;
	}
	return false;
}

bool DamageFormula::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

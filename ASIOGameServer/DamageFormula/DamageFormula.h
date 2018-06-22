#pragma once

struct Damage
{
	float combo1;
	float combo2;
	float combo3;
	float skill1;
	float skill2;
	float skill3;
	float growCombo1;
	float growCombo2;
	float growCombo3;
	float growSkill1;
	float growSkill2;
	float growSkill3;
};

struct Range
{
	float combo1;
	float combo2;
	float combo3;
	float skill1;
	float skill2;
	float skill3;
};

class DamageFormula
{
public:
	bool GetDamage(int code,Damage& damage);
	bool GetCurDamage(int code, FB::AttackState state, float level, float power, float& cur, float& formula);
	bool CheckRange(int attackerCode, FB::AttackState state,Vector3 from, Vector3 to);
private:
	std::map<int, Damage> damages;
	std::map<int, Range> ranges;

private:
	DamageFormula();
	~DamageFormula();
	static DamageFormula* inst;
public:
	static bool Create();
	static bool	Delete();
	static DamageFormula* Get() { return inst; }

};
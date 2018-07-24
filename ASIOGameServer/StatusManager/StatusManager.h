#pragma once


struct StatusInfo
{
	float baseHP;
	float basePower;
	float baseDefense;
	float baseSpeed;
	float growthHP;
	float growthPower;
	float growthDefense;
	float skill1_coolingtime;
	float skill2_coolingtime;
	float skill3_coolingtime;
};

class StatusManager
{
public:
	bool SetStatus(std::shared_ptr<class Character> character);	

private:
	std::map<int, StatusInfo> statusInfos;
	std::vector<int> expReqInfos;
private:
	StatusManager();
	~StatusManager();
	static StatusManager* inst;
public:
	static bool Create();
	static bool	Delete();
	static StatusManager* Get() { return inst; }

};
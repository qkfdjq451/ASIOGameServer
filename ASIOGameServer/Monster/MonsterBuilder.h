#pragma once

struct MonsterInfo
{
	uint code;
	string name;
	float HP;
	float Power;
};

class MonsterBuilder
{
public:
	static void Init();
	
	static  std::shared_ptr<class Monster> CreateMonster(int monstercode, int number);		
private:
	static std::map<int, MonsterInfo> infos;
	static class BTNode* InsertAI(std::shared_ptr<class Monster> monster);
};


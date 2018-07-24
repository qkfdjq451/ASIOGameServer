#pragma once


struct MonsterSpawnDesc
{
	int   monsterCode;
	int   monsterCount;
	Vector3 position;
	float distanceRange;
	float respawnTime;
	std::weak_ptr<class Navigation> navi;
};

class MonsterManager : public Component
{
public:
	friend class Channel;
private:

public:
	void Async_Function(std::shared_ptr<Func> func);
	void DamageProcessVector(shared_ptr<char[]> data,shared_ptr<class Character> character);
	void DamageProcess(shared_ptr<char[]> data, shared_ptr<class Character> character);

	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;
	
	int Make_FBB_All_Monster(vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>>&fbbArray, int slice = 10);
	void SetMovable(bool _b) { bMovable = _b; }
	void Add_Spawn_Monster(const MonsterSpawnDesc& desc);
	
private:
	bool bMovable;
	float saveTime; 

	std::vector<std::shared_ptr<Func>> req_func_list;
	//»ý¼ºÀÚ
public:
	MonsterManager();
	~MonsterManager();

private:
	int monKeyCount;
	map<int, weak_ptr<class Monster>> monsters;

};
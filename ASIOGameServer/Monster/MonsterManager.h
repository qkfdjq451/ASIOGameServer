#pragma once


struct MonsterSpawnDesc
{
	int   monsterCode;
	int   monsterCount;
	RespawnRange range;
	std::weak_ptr<class Navigation> navi;
};

class MonsterManager : public Component
{
public:
	friend class Channel;
private:
	

public:
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;

	
	std::shared_ptr<flatbuffers::FlatBufferBuilder> Make_FBB_All_Monster();
	void SetMovable(bool _b) { bMovable = _b; }
	void Add_Spawn_Monster(const MonsterSpawnDesc& desc);
private:

	bool bMovable;
	float saveTime; 

	//»ý¼ºÀÚ
public:
	MonsterManager();
	~MonsterManager();

private:
	int monKeyCount;
	map<int, weak_ptr<class Monster>> monsters;

};
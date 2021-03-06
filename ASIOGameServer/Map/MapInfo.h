#pragma once

struct Portal
{
	int destCode;
	int gateNumber;
	Vector3 position;
};

class MapInfo : public Component
{
public:
	friend class MapManager;

public:
	MapInfo();
	
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;

	
	bool InsertCharacter(std::shared_ptr<class Character> character);

	std::shared_ptr<class Channel> GetChannel(int index);
	const std::vector<struct MonsterSpawnDesc>& GetSpawnDesc(){ return spawnDescs; }

	MapType GetMapType() { return mapType; }
	int GetMapCode() { return mapCode; }
	std::shared_ptr<class Navigation> GetNavi() { return navi; }
	Portal* GetPortal(int gateNumber);
	std::string GetLevelName() { return LevelName; }
	Vector3 GetEscapeCoordinates() { return escapeCoordinates; }
private:
	int mapCode;
	Vector3 escapeCoordinates;
	std::string mapName;
	std::string LevelName;
	MapType mapType;
	std::shared_ptr<class Navigation> navi;

	std::vector<struct MonsterSpawnDesc> spawnDescs;
	std::map<int,std::weak_ptr<class Channel>> channels;
	std::map<int, Portal> portals;
	int maxUserCount;
};
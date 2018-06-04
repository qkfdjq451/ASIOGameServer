#pragma once

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
	std::vector<std::weak_ptr<class Channel>> channels;
private:
	int mapCode;
	std::string mapName;
	std::string LevelName;
	MapType mapType;
};
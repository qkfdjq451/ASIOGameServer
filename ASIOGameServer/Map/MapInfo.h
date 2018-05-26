#pragma once

class MapInfo :public Component, public std::enable_shared_from_this<MapInfo>
{
public:
	friend class MapManager;
	MapInfo();

private:
	int mapCode;
	std::string mapName;
	std::string LevelName;
	MapType mapType;
};
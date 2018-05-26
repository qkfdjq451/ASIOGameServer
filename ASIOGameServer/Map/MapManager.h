#pragma once
#include "../Component/Component.h"

class MapManager :public Component, public std::enable_shared_from_this<MapManager>
{
public:
	static std::map<std::string, MapType> MapTypes;
	MapType StringToMapType(std::string str);
	std::shared_ptr<class MapInfo> Get_MapInfo(int key);



private:
	std::map<int, std::shared_ptr<class MapInfo>> mapinfos;

public:
	MapManager();
	~MapManager();

};
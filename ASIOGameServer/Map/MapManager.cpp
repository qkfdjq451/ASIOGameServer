#include "../Global.h"

#include "MapManager.h"
#include "MapInfo.h"

void MapManager::BeginPlay()
{
	const char* query = "SELECT * FROM MapInfo;";
	auto vec = MySQLManager::Get()->Query_Vector(query);
	for (auto map : vec)
	{
		auto info = Component::CreateComponent<MapInfo>();
		if (info)
		{
			int code = atoi(map[0].second.c_str());
			info->mapCode = code;
			info->mapName = map[1].second;
			info->LevelName = map[2].second;
			info->mapType = StringToMapType(map[3].second);
			maps.insert(make_pair(code, info));
			Attach(info);
		}
	}
}

void MapManager::Tick()
{

}

void MapManager::EndPlay()
{

}

std::shared_ptr<MapInfo> MapManager::GetMapInfo(int code)
{
	auto retval = maps.find(code);
	if (retval != maps.end())
	{
		return retval->second.lock();
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Static
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::map<std::string, MapType> MapManager::MapTypes = { 
	{ std::string("Town"), MapType::Town},
{ std::string("Dungeon"), MapType::Dungeon }
};

MapType MapManager::StringToMapType(std::string str)
{
	auto result = MapTypes.find(str);
	if (result != MapTypes.end())
	{
		return result->second;
	}
	return MapType::None;
}

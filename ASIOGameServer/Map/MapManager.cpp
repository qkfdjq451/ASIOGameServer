#include "../Global.h"
#include "MapManager.h"
#include "MapInfo.h"

#include "../Mysql/MysqlManager.h"
std::map<std::string, MapType> MapManager::MapTypes;

MapType MapManager::StringToMapType(std::string str)
{
	auto result = MapTypes.find(str);
	if (result != MapTypes.end())
	{
		return result->second;
	}
	return MapType::None;
}

std::shared_ptr<class MapInfo> MapManager::Get_MapInfo(int key)
{
	auto result = mapinfos.find(key);
	if (result != mapinfos.end())
	{
		return result->second;
	}
	return nullptr;
}

MapManager::MapManager()
{	
	MapTypes.insert(std::make_pair(std::string("Town"), MapType::Town));
	MapTypes.insert(std::make_pair(std::string("Dungeoun"), MapType::Dungeoun));
	
	std::promise<MySQLManager::QVector> pro;
	std::future<MySQLManager::QVector> f = pro.get_future();

	auto func = std::make_shared<Function<std::promise<MySQLManager::QVector>&>>(
		[](std::promise<MySQLManager::QVector>& pro)
	{
		auto mm = WorkerGruop::Get_MySQL()->GetRoot()->GetComponent<MySQLManager>();
		const char* query = "SELECT * FROM MapInfo;";
		mm->Query_Result_Promise_Vector(query, std::ref(pro));
	}, std::ref(pro));

	//WorkerGruop::Get_MySQL()->PostFuction(func);
	std::thread  t = std::thread(std::bind(&Function<std::promise<MySQLManager::QVector>&>::func,func));
	t.detach();

	auto result = f.get();

	for (auto map : result)
	{
		auto info = std::make_shared<MapInfo>();
		int code = atoi(map[0].c_str());
		info->mapCode = code;
		info->mapName = map[1];
		info->LevelName = map[2];
		info->mapType = StringToMapType(map[3]);
		mapinfos.insert(std::make_pair(code, move(info)));
	}

}

MapManager::~MapManager()
{

}

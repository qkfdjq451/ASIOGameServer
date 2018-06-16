#include "../Global.h"
#include "MapInfo.h"
#include "../Channel/Channel.h"
#include "../User/UserManager.h"
#include "../Character/CharacterManager.h"
#include "../Character/Character.h"
#include "../Monster/MonsterManager.h"
#include "../Navi/Navigation.h"

MapInfo::MapInfo()
{
	navi = nullptr;
}

void MapInfo::BeginPlay()
{
	ifstream ifs;
	ifs.open(NaviDatas+"MapCode"+to_string(mapCode)+".navmesh", ios::in | ios::binary);
	if (ifs.fail())
	{
		cout << "실패" << endl;
	}
	else
	{
		navi = make_shared<Navigation>();
		cout << "성공" << endl;
		int size;
		ifs.read((char*)&size, sizeof(int));
		cout << size << endl;
		unsigned char* data = new unsigned char[size];
		ifs.read((char*)data, size);
		if (navi->Import(data, size))
		{
			cout << "임포트 성공" << endl;
		}	
	}
	ifs.close();

	if (mapType == MapType::Dungeon)
	{
		string query = "select SpawnRange.minX, SpawnRange.maxX, "
			"SpawnRange.minY, SpawnRange.maxY, MonsterSpawnInfo.MonsterCode ,"
			"MonsterSpawnInfo.MonsterCount  from MonsterSpawnInfo natural join SpawnRange where MonsterSpawnInfo.MapCode = ";		
		query += to_string(mapCode);
		auto vec = MySQLManager::Get()->Query(query.c_str());
		for (auto spawnInfo : vec)
		{
			MonsterSpawnDesc desc;
			for (auto data : spawnInfo)
			{
				if (data.first == "minX")
					desc.range.minX = stof(data.second);
				else if (data.first == "maxX")
					desc.range.maxX = stof(data.second);
				else if (data.first == "minY")
					desc.range.minY = stof(data.second);
				else if (data.first == "maxY")
					desc.range.maxY = stof(data.second);
				else if (data.first == "MonsterCode")
					desc.monsterCode = stoi(data.second);
				else if (data.first == "MonsterCount")
					desc.monsterCount = stoi(data.second);

				if (navi)
				{
					desc.navi = navi;
				}
			}
			spawnDescs.push_back(desc);
		}
	}

	for (int i = 1; i <= 3; i++)
	{
		auto channel = Component::CreateComponent<Channel>(mapCode,i);		
		Attach(channel);
		channels.push_back(channel);
	}
}

void MapInfo::PrevTick()
{

}

void MapInfo::Tick()
{
	PrevTick();

}

void MapInfo::EndPlay()
{

}

bool MapInfo::InsertCharacter(std::shared_ptr<class Character> character)
{
	vector<shared_ptr<Channel>> vec;
	GetComponents<Channel>(vec);

	for (auto ch : vec)
	{
		auto cm = ch->GetComponent<CharacterManager>();
		if (cm)
		{
			if (cm->GetCharacterCount() < Channel::Max_Character_Count)
			{				
				character->SetChannel(ch->GetNumber());
				return true;
			}
		}		
	}
	return false;
}

std::shared_ptr<class Channel> MapInfo::GetChannel(int index)
{
	if (channels.size() <= index || index < 0)
		return nullptr;
	return channels[index - 1].lock();
}

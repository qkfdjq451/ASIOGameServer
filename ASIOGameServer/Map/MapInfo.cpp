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

	//네비게이션 메쉬 임포트
	ifstream ifs;
	ifs.open(NaviDatas+"MapCode"+to_string(mapCode)+".navmesh", ios::in | ios::binary);
	if (ifs.fail())
	{
		//cout << "실패" << endl;
	}
	else
	{
		navi = make_shared<Navigation>();
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


	//몬스터 불러오기
	if (mapType == MapType::Dungeon)
	{
		char query[128];
		snprintf(query, 128,"Call GetSpawnInfo(%d)", mapCode);		
		auto vec = MySQLManager::Get()->Query_Vector(query);
		for (auto spawnInfo : vec)
		{
			MonsterSpawnDesc desc;
			desc.monsterCode = std::stoi(spawnInfo[2].second);
			desc.monsterCount = std::stoi(spawnInfo[3].second);
			desc.position = (Vector3(spawnInfo[4].second, spawnInfo[5].second, spawnInfo[6].second));
			desc.distanceRange = std::stof(spawnInfo[7].second);
			desc.respawnTime = std::stof(spawnInfo[8].second);
			desc.navi = navi;
			spawnDescs.push_back(desc);
		}
	}

	
	//채널 개설
	for (int i = 1; i <= 3; i++)
	{
		auto channel = Component::CreateComponent<Channel>(mapCode,i);		
		Attach(channel);
		channels.push_back(channel);
	}

	//포탈정보 불러오기
	char query[128];
	snprintf(query, 128, "Call GetAllPortalInMap(%d)", mapCode);
	auto vec = MySQLManager::Get()->Query_Vector(query);
	for (auto portal : vec)
	{
		Portal pt;
		pt.destCode = stoi(portal[2].second);
		pt.gateNumber = stoi(portal[3].second);
		pt.position = Vector3(portal[4].second, portal[5].second, portal[6].second);

		portals.insert(make_pair(pt.gateNumber, pt));
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
	if (channels.size() < index || index < 0)
		return nullptr;
	return channels[index - 1].lock();
}

Portal* MapInfo::GetPortal(int gateNumber)
{
	auto result=portals.find(gateNumber);
	if (result!= portals.end())
	{
		return &result->second;
	}
	return nullptr;
}

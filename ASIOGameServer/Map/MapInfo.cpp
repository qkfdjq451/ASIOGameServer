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
	//for (int i = 1; i <= 3; i++)
	//{
	//	auto channel = Component::CreateComponent<Channel>(mapCode,i);		
	//	Attach(channel);
	//	channels[i] = channel;
	//}

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
	// 비어 있는 채널 삭제
	for (auto iter= channels.begin();iter!= channels.end();)
	{
		auto channel = iter->second.lock();
		if (!channel)
		{
			channel->Distroy();
			printf("%d 채널 삭제 됨\n", iter->first);
			channels.erase(iter++);
			continue;
		}
		auto cm = channel->GetComponent<CharacterManager>();
		if (!cm ||cm->GetCharacterCount() == 0)
		{
			channel->AddTime(Time::Delta());
			if (channel->GetTime() >= 60)
			{
				channel->Distroy();
				printf("%d 채널 삭제 됨\n", iter->first);
				channels.erase(iter++);
				continue;
			}
		}
		else
		{
			channel->ResetTime();
		}
		++iter;
		
	}
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
	//기존 채널의 인원수 채크
	for (auto ch : channels)
	{
		auto channel = ch.second.lock();
		auto cm = channel->GetComponent<CharacterManager>();
		int temp = maxUserCount * 0.8;
		if (cm->GetCharacterCount() < temp)
		{	

			character->SetChannel(channel->GetNumber());
			return true;
		}
	}
	// 들어 갈 수 있는 채널이 없을 때 채널 개설

	// 비어 있는 채널 찾기
	int channelNumber = 1;
	while (true)
	{
		auto result = channels.find(channelNumber);
		if (result == channels.end())
			break;
		channelNumber++;
	}

	// 채널 생성
	auto channel = Component::CreateComponent<Channel>(mapCode, channelNumber);
	Attach(channel);
	channels[channelNumber] = channel;
	printf("%d 채널 생성\n", channelNumber);

	// 창설된 채널에 입장 허가
	character->SetChannel(channelNumber);
	return true;
}

std::shared_ptr<class Channel> MapInfo::GetChannel(int index)
{
	auto result = channels.find(index);
	if (result != channels.end())
	{
		return result->second.lock();
	}
	return nullptr;
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

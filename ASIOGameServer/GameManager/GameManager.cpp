#include "../Global.h"
#include "GameManager.h"

#include "../Map/MapManager.h"
#include "../Mysql/MysqlManager.h"



void GameManager::BeginPlay()
{
	Time::Create();
	auto mapmanager = Component::CreateComponent<MapManager>();
	if (mapmanager)
	{
		mapmanager->SetTag("MapManager");
		Attach(mapmanager);
	}
}

void GameManager::Tick()
{
	Time::Get()->Update();
}

void GameManager::EndPlay()
{
	Time::Delete();
}



GameManager::GameManager()
{

}

GameManager::~GameManager()
{

}

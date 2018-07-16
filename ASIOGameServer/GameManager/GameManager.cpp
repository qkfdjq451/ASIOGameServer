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
	//static float tick = 0;
	//static float time = 0;
	//time += Time::Get()->Delta();
	//++tick;
	//if (time > 1)
	//{
	//	time = 0;
	//	system("cls");
	//	cout << "ÇÁ·¹ÀÓ : " << tick << endl;
	//	tick = 0;
	//}
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

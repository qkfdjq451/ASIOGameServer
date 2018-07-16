#include "../Global.h"
#include "ItemManager.h"

ItemManager* ItemManager::inst = nullptr;

const std::shared_ptr<Item> ItemManager::GetItem(int key)
{
	auto result =iteminfos.find(key);
	if (result != iteminfos.end())
	{
		return result->second;
	}
	return nullptr;
}

ItemManager::ItemManager()
{
	string query = "Call GetItemlist()";
	MySQLManager::Get()->Async_Query(query.c_str(),
		[this](QVector& vec, bool result)
	{
		//정상적으로 받았을 때
		if (result)
		{
			for (auto datas : vec)
			{
				std::shared_ptr<Item> item=make_shared<Item>();
				item->itemName = datas[1].second.c_str();
				item->type = (ItemType)stoi(datas[2].second);
				float* temp = (float*)&item->hp;
				for (int i = 0; i < datas.size()-3; i++)
				{
					temp[i] = stof(datas[i + 3].second);
				}
				iteminfos.insert(make_pair(stoi(datas[0].second), item));
			}
		}
		else
		{
			//예외처리
		}
	});
}

ItemManager::~ItemManager()
{
}

bool ItemManager::Create()
{
	if (inst == nullptr)
	{
		inst = new ItemManager();
		return true;
	}
	return false;
}

bool ItemManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}

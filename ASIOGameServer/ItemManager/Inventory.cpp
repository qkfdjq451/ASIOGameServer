#include "../Global.h"
#include "Inventory.h"
#include "ItemManager.h"

std::shared_ptr<Slot> Inventory::GetSlot(int key)
{
	auto result = slots.find(key);
	if (result!= slots.end())
	{
		return result->second;
	}		
	return nullptr;
}

int Inventory::GetEmptySlot()
{
	for (int i = 1; i<=maxSlotCount;i++)
	{
		auto result = slots.find(i);
		if (result == slots.end())
		{
			return i;
		}
	}
	return -1;
}

void Inventory::SetInventory(int characterCode)
{
	string query = "Select * From Inventory Where Inventory.CharacterCode = ";
	query += to_string(characterCode);

	MySQLManager::Get()->Async_Query(query.c_str(),
		[this](QVector& vec, bool result)
	{
		//정상적으로 받았을 때
		
		if (result)
		{
			std::map<int, int> userItemKey_slot;
			//
			money = stoi(vec[0][1].second);
			for (auto row : vec)
			{
				for (int i = 1; i <= maxSlotCount; i++)
				{					
					int useritemkey = stoi(row[i+1].second);
					if (useritemkey != 0)
					{
						slots[i] = make_shared<Slot>();
						slots[i]->userItemKey = useritemkey;
						userItemKey_slot.insert(make_pair(useritemkey, i));
					}
				}
			}			
			if (slots.size()==0)
			{
				return;
			}
			// User가 가지고 있는 아이템 정보를 디비에서 가져온다.
			bool first = true;
			string query2 = "Select UserItems.UserItemCode, UserItems.ItemCode,UserItems.Count From UserItems WHERE ";
			for (auto slot : slots)
			{
				string add_where = "";
				if (first)
				{
					first = false;
				}
				else
				{
					add_where.append(" or ");
				}
				add_where.append("UserItems.UserItemCode = ");
				add_where.append(to_string(slot.second->userItemKey));
				query2 += add_where;
			}
			auto table = MySQLManager::Get()->Query_Vector(query2.c_str());
			for (auto row : table)
			{
				int userItemKey = stoi(row[0].second);
				auto result = userItemKey_slot.find(userItemKey);
				if (result != userItemKey_slot.end())
				{
					auto iter = slots.find(result->second);
					if (iter != slots.end())
					{
						auto slot =iter->second;
						slot->itemKey = stoi(row[1].second);
						slot->count = stoi(row[2].second);
						slot->itemInfo=ItemManager::Get()->GetItem(stoi(row[1].second));
					}
				}
			}
		}
		else
		{
			//예외처리
		}
	});
}

Inventory::Inventory()
{
	maxSlotCount = 30;
}

shared_ptr<flatbuffers::FlatBufferBuilder> Inventory::Make_Inventory_FBB()
{
	auto fbb = make_shared<flatbuffers::FlatBufferBuilder>();
	vector<flatbuffers::Offset<FB::Slot>> slot_vector;
	for (auto slot : slots)
	{
		auto itemInfo = slot.second->itemInfo.lock();
		if (itemInfo != nullptr)
		{
			auto slotB = FB::SlotBuilder(*fbb);
			slotB.add_slotNumber(slot.first);
			//TODO : 아이템 고유번호 보내주기
			//slotB.add_itemCode(itemInfo->)
		}
	}
	auto slot_vector_offset = fbb->CreateVector(slot_vector);
	auto InventoryB = FB::InventoryBuilder(*fbb);
	InventoryB.add_slot_vec(slot_vector_offset);
	InventoryB.add_money(money);
	fbb->Finish(InventoryB.Finish());

	return fbb;
}

#pragma once
#include "ItemManager.h"

struct Slot
{
	UINT64 userItemKey;
	int itemKey;
	int count;
	std::weak_ptr<Item> itemInfo;
};

class Inventory
{
public:
	void SetInventory(int characterCode);

	std::shared_ptr<Slot> GetSlot(int key);
	int GetEmptySlot();
	Inventory();

	shared_ptr<flatbuffers::FlatBufferBuilder> Make_Inventory_FBB();
private:
	int money;
	int maxSlotCount;
	std::map<int, shared_ptr<Slot>> slots;
};
#pragma once
enum class ItemType : short
{
	Weapon_Dagger=1,
	Weapon_Gun,
	Armor,
	Shoes,
	Accessory,
	Consume,
	Sundry
};

struct Item
{
	string itemName;
	ItemType type;
	float hp;
	float mp;
	float power;
	float defense;
	float speed;
	float time;
	float price;
	void const Copy(std::shared_ptr<Item> item)
	{		
		item->itemName = itemName;
		item->type = type;
		item->hp = hp;
		item->mp = mp;
		item->power = power;
		item->defense = defense;
		item->speed = speed;
		item->time = time;
		item->price = price;
	};
};

class ItemManager
{
public:
	const std::shared_ptr<Item> GetItem(int key);
private:	
	std::map<int, std::shared_ptr<Item>> iteminfos;
////////////////////	ΩÃ±€≈Ê
private:
	ItemManager();
	~ItemManager();
	static ItemManager* inst;
public:
	static bool Create();
	static bool	Delete();
	static ItemManager* Get() { return inst; }

};
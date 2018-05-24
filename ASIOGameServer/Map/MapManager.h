#pragma once
#include "../Component/Component.h"

class MapManager : public Component
{
public:


private:
	std::map<int, std::unique_ptr<class MapInfo>> mapinfo;

public:
	MapManager();
	~MapManager();

};
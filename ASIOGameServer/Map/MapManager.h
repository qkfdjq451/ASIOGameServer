#pragma once

class MapManager : public Component
{
	void BeginPlay() override;
	void Tick() override;
	void EndPlay() override;

public:
	std::shared_ptr<class MapInfo> GetMapInfo(int code);
private:
	std::map<int, std::weak_ptr<MapInfo>> maps;

	static std::map<std::string, MapType> MapTypes;
public:
	static MapType StringToMapType(std::string str);
};
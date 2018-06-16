#pragma once
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};


class Navigation
{
public:
	unsigned char* navData;
	int navDataSize;


	bool Import(unsigned char* navData, int navDataSize);
	Navigation();
	~Navigation();

	class dtNavMeshQuery* GetQuery() { return m_navQuery; }
private:
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;


//Static
public:
	static void Init();
	static dtQueryFilter GetQueryFilter() { return filter; }
	static mt19937_64 rnd;
	static float frand();
	static float frand2();
private:
	static dtQueryFilter filter;
	static random_device rn;
};

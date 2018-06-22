#include "../Global.h"
#include "Navigation.h"
random_device Navigation::rn;
 mt19937_64 Navigation::rnd(rn());
 dtQueryFilter Navigation::filter;
 Vector3 Navigation::m_polyPickExt(2,4,2);

bool Navigation::Import(unsigned char * navData, int navDataSize)
{
	this->navData = navData;
	this->navDataSize = navDataSize;

	m_navMesh = dtAllocNavMesh();
	m_navQuery = dtAllocNavMeshQuery();
	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
		return false;
	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
		return false;


	return true;
}

Navigation::Navigation()
{
	m_navMesh = nullptr;
	m_navQuery = nullptr;
	navData = nullptr;
	navDataSize = 0;
}

Navigation::~Navigation()
{
	if (navData)
		delete navData;
}

void Navigation::Init()
{
	srand(time(0));
	filter.setIncludeFlags(SamplePolyFlags::SAMPLE_POLYFLAGS_WALK);
}


float Navigation::frand()
{
	return (float)rand() / (float)RAND_MAX;
}

float Navigation::frand2()
{
	uniform_real_distribution<float> range(MIN_flt, MAX_flt);
	return range(rnd);
}

#include "../Global.h"
#include "MapInfo.h"
#include "../User/UserManager.h"

MapInfo::MapInfo()
{
	Attach(move(std::make_shared<UserManager>()));
}

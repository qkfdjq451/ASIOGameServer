#include "../Global.h"
#include "WorkerGruop.h"
#include "../Asio/Session/SessionManager.h"
#include "../User/UserManager.h"
#include "../Component/Component.h"
#include "../Map/MapManager.h"

#include "../Mysql/MysqlManager.h"

std::unique_ptr<WorkerGruop> WorkerGruop::ioGroup = nullptr;
std::unique_ptr<WorkerGruop> WorkerGruop::ingameGroup = nullptr;
std::unique_ptr<WorkerGruop> WorkerGruop::mysqlGroup = nullptr;

void WorkerGruop::Create(asio::io_service& io)
{
	mysqlGroup = std::make_unique<WorkerGruop>(io);
	mysqlGroup.get()->GetRoot()->Attach(move(std::make_shared<MySQLManager>()));

	ioGroup = std::make_unique<WorkerGruop>(io);
	ioGroup.get()->GetRoot()->Attach(move(std::make_shared<SessionManager>()));
	ioGroup.get()->GetRoot()->Attach(move(std::make_shared<UserManager>()));

	ingameGroup= std::make_unique<WorkerGruop>(io);
	ingameGroup.get()->GetRoot()->Attach(move(std::make_shared<MapManager>()));
}


WorkerGruop::WorkerGruop(asio::io_service & io)
	:strand_(io)
{
	root = std::make_shared<Component>();
}

WorkerGruop::~WorkerGruop()
{

}

void WorkerGruop::PostFuction(std::shared_ptr<Func> func)
{
	strand_.post([func]
	{
		func->func();
	});
	return;
}




#include "../Global.h"
#include "WorkerGroup.h"
#include "../Asio/Session/SessionManager.h"
#include "../User/UserManager.h"
#include "../Component/Component.h"

#include "../Mysql/MysqlManager.h"

std::shared_ptr<WorkerGroup> WorkerGroup::ioGroup = nullptr;


void WorkerGroup::Create(asio::io_service& io)
{
	ioGroup = Component::CreateComponent<WorkerGroup>(io);
	ioGroup = std::make_unique<WorkerGroup>(io);
	ioGroup->SetWorkerGroup(ioGroup.get());
	ioGroup->Attach(move(std::make_shared<SessionManager>()));
	ioGroup->Attach(move(std::make_shared<UserManager>()));
}

WorkerGroup::WorkerGroup(asio::io_service & io)
	:strand_(io)
{	
}

WorkerGroup::~WorkerGroup()
{
		
}

void WorkerGroup::PostFuction(std::shared_ptr<Func> func)
{
	strand_.post([func]
	{
		func->func();
	});
	return;
}




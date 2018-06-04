#include "../Global.h"
#include "Asio.h"
#include "./Session/Session.h"
#include "./Session/SessionManager.h"
#include "../User/UserManager.h"
#include "../User/User.h"

#include "../Map/MapInfo.h"
#include "../Map/MapManager.h"


server::server(asio::io_service & io_service, short port)
	: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	socket_(io_service)
{
	do_accept();
}

void server::NewUser(std::shared_ptr<SUTuple> su)
{
	static int key = 0;
	auto sm = su->sm.lock();
	auto um = su->um.lock();
	auto socket =std::move(su->socket);
	if (sm == nullptr || um == nullptr)
	{
		return;
	}
	auto session = sm->CreateSession(std::move(socket), key);
	auto user = um->CreateUser(key);

	user->SetSession(session);
	session->SetUser(user);


	key++;
}

void server::do_accept()
{
	//클라이언트 접속 대기	
	acceptor_.async_accept(socket_,
		[this](std::error_code ec) 
	{
		if (!ec) 
		{
			auto root = WorkerGroup::Get_IOGroup();
			auto sm = root->GetComponent<SessionManager>();
			auto um = root->GetComponent<UserManager>();

			auto su=std::make_shared<SUTuple>(sm, um, std::move(socket_));

			auto newuserFunc = std::make_shared<Function<std::shared_ptr<SUTuple >>>(
				bind(&server::NewUser, this, std::placeholders::_1),move(su));
			WorkerGroup::Get_IOGroup()->PostFuction(newuserFunc);
		}
		//재귀호출이 아님.
		do_accept();
	});
}

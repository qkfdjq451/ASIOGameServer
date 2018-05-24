
#include "User.h"
#include "../Asio/Session/Session.h"

User::User()
{
}

void User::SetSession(std::shared_ptr<class Session> _session)
{
	session = _session;
}

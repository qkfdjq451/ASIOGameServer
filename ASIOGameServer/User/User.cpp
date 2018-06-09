
#include "User.h"
#include "../Character/Character.h"

#include "../Asio/Session/Session.h"


User::User()
	:character(nullptr)
{
	

}

User::~User()
{
	if (character)
	{
		character->SetMapKey(0);
		character->SetChannel(0);
		character->Clear();
	}
}

void User::SetSession(std::shared_ptr<class Session> _session)
{
	session = _session;
}

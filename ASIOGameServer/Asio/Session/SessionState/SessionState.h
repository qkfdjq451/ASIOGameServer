#pragma once

#include "../Session.h"

class SessionState
{
public:
	friend class Session;
	SessionState(std::shared_ptr<class Session> session);
	std::weak_ptr<Session> GetSession() { return session; }
protected:
	std::weak_ptr<Session> session;
	virtual void On_Read(const PS& symbol, void* recv_data, unsigned short size) = 0;
};

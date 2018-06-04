#pragma once
#include "SessionState.h"

class LoginState :public SessionState
{
public:
	LoginState(std::shared_ptr<class Session> session) :SessionState(session) {}
protected:
	void On_Read(const PS& symbol, void* recv_data, unsigned short size);
};


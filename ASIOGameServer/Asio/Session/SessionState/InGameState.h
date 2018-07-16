#pragma once
#include "SessionState.h"

class InGameState :public SessionState
{
public:
	InGameState(std::shared_ptr<class Session> session) :SessionState(session) {}
protected:
	void On_Read(const PS& symbol, void* recv_data, unsigned short size);

	std::shared_ptr<class Channel> GetChannel(std::shared_ptr<class Character> character);


};


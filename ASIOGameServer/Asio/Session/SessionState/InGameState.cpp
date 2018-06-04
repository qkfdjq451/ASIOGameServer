#include "../../../Global.h"
#include "InGameState.h"

#include "../../../User/User.h"
#include "../../../User/UserManager.h"
#include "../../../Map/MapManager.h"
#include "../../../Map/MapInfo.h"
#include "../../../Character/Character.h"

void InGameState::On_Read(const PS& symbol, void* recv_data, unsigned short size)
{
	
	switch (symbol)
	{
	case PS::CHATTING_ALL:
		{
			auto ss = session.lock();
			auto user = ss->GetUser().lock();

			if (ss != nullptr&&user != nullptr)
			{
			}
		}
		break;
	case PS::REQ_ENTER_MAP:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();

		if (ss != nullptr&&user != nullptr)
		{
			auto map = FB::GetMap(recv_data);			
			user->GetCharacter()->SetMapKey(map->mapcode());
		}
	}
		break;
	case PS::REQ_ENTER_CHANNEL:
	{
		auto ss = session.lock();		auto user = ss->GetUser().lock();

		if (ss != nullptr&&user != nullptr)
		{
			auto map = FB::GetMap(recv_data);
			user->GetCharacter()->SetChannel(map->channel());
		}
	}	
		break;
	case PS::PING_TEST:
	{
		auto ss = session.lock();
		ss->PushSend(PS::PING_TEST, recv_data, size);		

		break;
	}

	default:
		break;
	}
}

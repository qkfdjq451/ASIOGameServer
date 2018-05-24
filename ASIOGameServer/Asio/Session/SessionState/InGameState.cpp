#include "../../../Global.h"
#include "InGameState.h"
#include "../../../User/User.h"

void InGameState::On_Read(const PS& symbol, void* recv_data, unsigned short size)
{
	using namespace std;
	switch (symbol)
	{
	case PS::CHATTING_ALL:
		{
			printf("Ã¤ÆÃ\n");

			auto ss = session.lock();
			auto user = ss->GetUser().lock();
			
			if (ss != nullptr&&user!=nullptr)
			{
				auto chat = FBChat::GetChat(recv_data);

				auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
				auto nick = fbb->CreateString(user->GetName());
				auto message = fbb->CreateString(chat->message()->c_str());
				auto chatb = FBChat::ChatBuilder(*fbb);
				chatb.add_nick(nick);
				chatb.add_message(message);

				auto result = chatb.Finish();
				fbb->Finish(result);
				
				ss->PushSend(PS::CHATTING_ALL, move(fbb));
			}
		}
		break;

	case PS::PING_TEST:
	{
		auto ss = session.lock();
		ss->PushSend(PS::PING_TEST, recv_data, size);
		//auto sb = new SendBuffer(PS::CHATTING_ALL);

		break;
	}

	default:
		break;
	}
}

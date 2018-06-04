#include "../../Global.h"
#include "SessionManager.h"
#include "Session.h"



std::shared_ptr<class Session> SessionManager::CreateSession(tcp::socket socket, int key)
{
	try
	{
		auto result = sessions.find(key);
		if (result == sessions.end())
		{
			auto session = std::make_shared<Session>(std::move(socket));
			if (session != nullptr)
			{
				session->Init();
				session->start();
				session->key = key;
				sessions.insert(std::make_pair(key, session));
				return session;
			}
		}
	}
	catch (std::exception e)
	{
		
	}
	return nullptr;
}

//void SessionManager::Send_All_Session(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
//{	
//		for (auto & session : sessions)
//		{
//			//session.second->PushSend(fbb);
//		}
//}

bool SessionManager::RemoveSocket(int key)
{
	auto result = sessions.find(key);
	if (result == sessions.end())	
		return false;
	sessions.erase(key);
	printf("%dÁö¿öÁü\n",key);
	return true;
}

SessionManager::SessionManager()	
{

}

SessionManager::~SessionManager()
{
}

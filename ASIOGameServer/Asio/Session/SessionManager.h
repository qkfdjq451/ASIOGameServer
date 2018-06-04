#pragma once

class SessionManager : public Component
{
public:
	std::shared_ptr<class Session> CreateSession(tcp::socket socket,int key);

	//void Send_All_Session(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	bool RemoveSocket(int key);

private:	
	std::map<int, std::shared_ptr<class Session>> sessions;

public:
	SessionManager();
	~SessionManager();
};

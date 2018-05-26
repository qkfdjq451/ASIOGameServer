#pragma once

class Session : public std::enable_shared_from_this<Session> 
{
public:
	friend class SessionManager;
	
	Session(tcp::socket socket);
	~Session();
	
	void start();
public:
	void Init();
	
	void SetUser(std::shared_ptr<class User> user);

	//POST
	void PushSend(std::shared_ptr<struct SendBuffer> sb);
	void PushSend(const PS& symbol, void* buffer_pointer, short buffersize);
	void PushSend(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	std::weak_ptr<class User> GetUser() { return user; }

private:
	int key;
	//실제로 송수신하는 함수
	void do_read();
	void do_write();
	

	//Recv결과 처리 및 buffer컨트롤 하는 함수
	EResult RecvResult(std::size_t length);
	EResult SendResult(SendBuffer& send_buffer, std::size_t length);

	tcp::socket socket_;					//자신의 소켓
	asio::steady_timer timer_;				//타이머.
	asio::io_service::strand strand_;	//스트랜드

	std::weak_ptr<class User> user;
	
	RecvBuffer recv_buffer;
	unsigned short com_recvSize;

	std::queue<std::shared_ptr<SendBuffer>> send_buffers;
	std::shared_ptr<SendBuffer> current_send_buffer;

	unsigned short com_sendSize;
private:
	//State
	class SessionState* currentState;
	class InGameState* ingameState;

};

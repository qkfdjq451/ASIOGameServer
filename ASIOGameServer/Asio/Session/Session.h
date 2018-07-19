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
	void PushSend(const PS& symbol);
	void PushSend(const PS& symbol, void* buffer_pointer, short buffersize);
	void PushSend(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	std::weak_ptr<class User> GetUser() { return user; }

private:
	int key;
	//������ �ۼ����ϴ� �Լ�
	void do_read();
	void do_write();
	

	//Recv��� ó�� �� buffer��Ʈ�� �ϴ� �Լ�
	EResult RecvResult(std::size_t length);
	EResult SendResult(std::shared_ptr<struct SendBuffer> sb, std::size_t length,std::size_t& current_complete_length);

	tcp::socket socket_;					//�ڽ��� ����
	asio::steady_timer timer_;				//Ÿ�̸�.
	asio::io_service::strand strand_;	//��Ʈ����

	std::weak_ptr<class User> user;
	
	RecvBuffer recv_buffer;
	unsigned short com_recvSize;
	//SendBuffer Complete Size
	std::queue<std::pair<std::shared_ptr<SendBuffer>,size_t>> send_buffers;
	//std::shared_ptr<SendBuffer> current_send_buffer;

	//unsigned short com_sendSize;

	//����
public:
	void SetState(ESessionState _state);

private:
	//State
	class SessionState* currentState;
	class InGameState* ingameState;
	class LoginState* loginState;

};

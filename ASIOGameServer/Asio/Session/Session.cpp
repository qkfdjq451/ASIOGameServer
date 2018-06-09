#include "../../Global.h"
#include "Session.h"
#include "SessionManager.h"
#include "SessionState/InGameState.h"
#include "SessionState/LoginState.h"

#include "../../User/User.h"
#include "../../User/UserManager.h"

Session::Session(tcp::socket socket)
	: socket_(std::move(socket)),
	timer_(socket_.get_io_service()),
	strand_(socket_.get_io_service())
{
}

Session::~Session()
{
	
}


void Session::start()
{
	do_read();
}

void Session::Init()
{
	current_send_buffer = nullptr;
	auto pThis = shared_from_this();
	ingameState = new InGameState(pThis);
	loginState = new LoginState(pThis);
	currentState = loginState;
	com_recvSize = 0;
	com_sendSize = 0;
}



void Session::do_read()
{
	auto self(shared_from_this());
	strand_.post([this, self]()
	{
		socket_.async_read_some(asio::buffer(recv_buffer.buffer+ com_recvSize, BUFSIZE)
			, [this, self](std::error_code ec, std::size_t length)
		{
			printf(" read 받은 크기 : %zd\n" , length);
			//소켓 종료 요청
			if (length == 0)
			{
				//printf("Close\n");				
				auto func = std::make_shared<Function<void>>(
					[this, self]()
				{
					auto sm = WorkerGroup::Get_IOGroup()->GetComponent<SessionManager>();
					auto um = WorkerGroup::Get_IOGroup()->GetComponent<UserManager>();
					sm->RemoveSocket(key);
					um->RemoveUser(key);
				});
				WorkerGroup::Get_IOGroup()->PostFuction(move(func));
				return;
			}

			auto retval = RecvResult(length);
			
			switch (retval)
			{
			case EResult::LOW:				
				printf("조금 받음\n");
				break;
			case EResult::COMPLETE:
				printf("정상적으로 받음\n");
				//recv_buffer.packet.symbol
				currentState->On_Read(recv_buffer.packet.symbol,&recv_buffer.packet.buffer, recv_buffer.packet.size);
				com_recvSize = 0;
				break;
			case EResult::OVER:
				printf("Over해서 받음\n");
				while (com_recvSize>recv_buffer.packet.size)
				{
					currentState->On_Read(recv_buffer.packet.symbol,&recv_buffer.packet.buffer, recv_buffer.packet.size);
					memmove(recv_buffer.buffer, recv_buffer.buffer + recv_buffer.packet.size, com_recvSize - recv_buffer.packet.size);
					com_recvSize -= recv_buffer.packet.size;
				}				
				break;
			}
			do_read();
		});
	});
}

//
void Session::do_write()
{
	auto self(shared_from_this());

	strand_.post([this, self]()
	{	
		std::shared_ptr<SendBuffer> temp;

		if (current_send_buffer == nullptr)
		{
			if (send_buffers.empty() == true)
			{
				return;
			}
			else
			{
				current_send_buffer = send_buffers.front();	
				temp = current_send_buffer;
			}
		}
		
		socket_.async_write_some(asio::buffer(
			&current_send_buffer->buffer.start_pointer + com_sendSize,
			current_send_buffer->buffer.packet.size - com_sendSize)
		,[this, self, temp](std::error_code ec, std::size_t length)
		{
			EResult retval;
			if (current_send_buffer)
			{
				retval = SendResult(current_send_buffer, length);
			}
			else
			{
				retval = EResult::None;
			}

			printf(" send 크기 : %zd\n", length);
			
			if (length == 0)
			{
				return;
			}

			switch (retval)
			{
			case EResult::LOW:
				do_write();
				break;
			case EResult::COMPLETE:				
				com_sendSize = 0;				
				current_send_buffer = nullptr;

				send_buffers.pop();
				do_write();
				break;

			case EResult::None:
			{
				com_sendSize = 0;
				cout << "current_send_buffer 지워짐 " << endl;
			}
			break;
			}
			
		});
	});
}

EResult Session::RecvResult( std::size_t length)
{
	//만일 보낸 데이터와 전송이 완료된 데이터의 크기가 같다면 low 리턴
	com_recvSize += length;
	if (com_recvSize < sizeof(unsigned short))
	{
		return EResult::LOW;
	}
	//받아야할 데이터와 받은 데이터의 양이 같을 때 
	if (recv_buffer.packet.size == com_recvSize)
	{
		return EResult::COMPLETE;
	}
	//받아야할 데이터의 양이 더 적을 때
	else if (recv_buffer.packet.size > com_recvSize)
	{
		return EResult::LOW;
	}
	//넘치게 받았을 떄
	return EResult::OVER;
}

EResult Session::SendResult(std::shared_ptr<struct SendBuffer> sb,std::size_t length)
{
	com_sendSize += length;
	if (com_sendSize < sizeof(unsigned short))
	{
		return EResult::LOW;
	}
	//받아야할 데이터와 받은 데이터의 양이 같을 때 
	if (sb->buffer.packet.size == com_sendSize)
	{
		return EResult::COMPLETE;
	}
	return EResult::LOW;
}

void Session::SetState(ESessionState _state)
{
	switch (_state)
	{
	case ESessionState::Login:
		currentState = loginState;
		break;
	case ESessionState::InGame:
		currentState = ingameState;
		break;
	}
}

void Session::PushSend(std::shared_ptr<struct SendBuffer> sb)
{
	auto self(shared_from_this());
	strand_.post([this, self, sb]
	{
		if (sb == nullptr)
		{
			std::cout << "널 피티알!!" << std::endl;
			return;
		}			
		std::cout << "널아님!!" << std::endl;
		send_buffers.push(sb);
		do_write();
	});
}

void Session::PushSend(const PS & symbol)
{
	auto self(shared_from_this());

	strand_.post([this, self,  symbol]
	{
		send_buffers.push(std::make_shared<SendBuffer>(symbol));
		do_write();
	});
}

void Session::PushSend(const PS & symbol, void * buffer_pointer, short buffersize)
{
	auto self(shared_from_this());

	strand_.post([this, self, buffer_pointer, symbol, buffersize]
	{
		send_buffers.push(std::make_shared<SendBuffer>(symbol, buffer_pointer, buffersize));
		do_write();
	});

}

void Session::PushSend(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
{
	auto self(shared_from_this());

	strand_.post([this, self, symbol,fbb]
	{
		send_buffers.push(std::make_shared<SendBuffer>(symbol,move(fbb)));
		do_write();
	});
}

void Session::SetUser(std::shared_ptr<class User> _user)
{
	user = _user;
}

//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once


//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define ASIO_STANDALONE
#define ASIO_HEADER_ONLY

#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#define ASIO_HAS_VARIADIC_TEMPLATES
#define ASIO_HAS_STD_FUNCTION
#define ASIO_HAS_STD_CHRONO
#define ASIO_HAS_STD_ATOMIC
#define BOOST_ALL_NO_LIB



#include <asio.hpp>
#define _WINSOCK_DEPRECATED_NO_WARNINGS


using asio::ip::tcp;

class server 
{
public:
	struct SUTuple
	{
		std::weak_ptr<class SessionManager> sm;
		std::weak_ptr<class UserManager> um;
		tcp::socket socket;
		SUTuple(std::weak_ptr<class SessionManager> sm, std::weak_ptr<class UserManager> um, tcp::socket socket)
			:sm(sm),um(um),socket(std::move(socket)){}
	};

	server(asio::io_service& io_service, short port);
private:
	
	void NewUser(std::shared_ptr<SUTuple> su);

	void do_accept();
	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

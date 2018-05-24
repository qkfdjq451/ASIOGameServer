#pragma once

#include <flatbuffers/flatbuffers.h>


#include "./FlatBuffer/FBChat_generated.h"
#include "./FlatBuffer/FBPlayer_generated.h"

enum class Packet_Symbal : unsigned short
{
	PING_TEST,
	CHATTING_ALL,
	CHATTING_WHISPER,

	TEST
};

typedef Packet_Symbal PS;

struct Ping
{
	int64_t time;
};
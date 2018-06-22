#pragma once

#include <flatbuffers/flatbuffers.h>


#include "./FlatBuffer/FB_generated.h"
#include "./FlatBuffer/FB_chat_generated.h"
#include "./FlatBuffer/FB_Character_generated.h"
#include "./FlatBuffer/FB_Character_Vec_generated.h"
#include "./FlatBuffer/FB_User_generated.h"
#include "./FlatBuffer/FB_Map_generated.h"
#include "./FlatBuffer/FB_Move_generated.h"
#include "./FlatBuffer/FB_Move_Vec_generated.h"
#include "./FlatBuffer/FB_Attack_generated.h"
#include "./FlatBuffer/FB_Damage_generated.h"
#include "./FlatBuffer/FB_Damage_Vec_generated.h"

#include "./FlatBuffer/FB_Monster_generated.h"
#include "./FlatBuffer/FB_Monster_Vec_generated.h"

enum class Packet_Symbal : unsigned short
{
	PING_TEST,
	CHATTING_ALL,
	CHATTING_WHISPER,

	ENTER_NEW_CHARACTER,
	ENTER_NEW_CHARACTER_VECTOR,
		
	LEAVE_CHARACTER,
	LEAVE_CHARACTER_VECTOR,

	REQ_SELECT_CHARACTER,
	CON_SELECT_CHARACTER,
	
	REQ_ENTER_MAP,
	CON_ENTER_MAP,

	REQ_ENTER_CHANNEL,
	CON_ENTER_CHANNEL,
	
	//이동관련
	MOVING,
	MOVING_VECTOR,

	//몬스터 관련
	RESPAWN_MONSTER,
	RESPAWN_MONSTER_VEC,

	MONSTER_MOVING_VECTOR,

	//공격관련
	ATTACK,
	REQ_DAMAGE,
	CON_DAMAGE,

	REQ_DAMAGE_VECTOR,
	CON_DAMAGE_VECTOR,

	TEST
};

typedef Packet_Symbal PS;

struct Ping
{
	int64_t time;
};
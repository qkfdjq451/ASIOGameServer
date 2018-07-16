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
#include "./FlatBuffer/FB_Die_generated.h"
#include "./FlatBuffer/FB_Portal_generated.h"

#include "./FlatBuffer/FB_Monster_generated.h"
#include "./FlatBuffer/FB_Monster_Vec_generated.h"

#include "./FlatBuffer/FB_Slot_generated.h"
#include "./FlatBuffer/FB_Inventory_generated.h"

enum class Packet_Symbal : unsigned short
{
	//로그인, 케릭터 선택 관련
	REQ_SELECT_CHARACTER,
	CON_SELECT_CHARACTER,
	

	//맵 채널 이동 관련
	REQ_ENTER_MAP,
	CON_ENTER_MAP,

	REQ_ENTER_CHANNEL,
	CON_ENTER_CHANNEL,

	//인게임 관련
	PING_TEST,

	//채팅 관련
	CHATTING_ALL,
	CHATTING_WHISPER,

	//인게임 내 클라이언트 요청 패킷 관련
	REQ_CHARACTER_LIST,
	REQ_MONSTER_LIST,

	//케릭터 
	ENTER_NEW_CHARACTER,
	ENTER_NEW_CHARACTER_VECTOR,

	LEAVE_CHARACTER,
	LEAVE_CHARACTER_VECTOR,

	//캐릭터 이동관련
	MOVING,
	MOVING_VECTOR,

	//몬스터 관련
	RESPAWN_MONSTER,
	RESPAWN_MONSTER_VEC,

	//몬스터 이동
	MONSTER_MOVING_VECTOR,

	//공격관련
	ATTACK,
	MONSTER_ATTACK,
	REQ_DAMAGE,
	CON_DAMAGE,

	DIE,

	REQ_DAMAGE_VECTOR,
	CON_DAMAGE_VECTOR,

	//포탈
	REQ_PORTAL,
	CON_PORTAL,

	//게임
	


	TEST
};

typedef Packet_Symbal PS;

struct Ping
{
	int64_t time;
};
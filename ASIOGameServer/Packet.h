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
	//�α���, �ɸ��� ���� ����
	REQ_SELECT_CHARACTER,
	CON_SELECT_CHARACTER,
	

	//�� ä�� �̵� ����
	REQ_ENTER_MAP,
	CON_ENTER_MAP,

	REQ_ENTER_CHANNEL,
	CON_ENTER_CHANNEL,

	//�ΰ��� ����
	PING_TEST,

	//ä�� ����
	CHATTING_ALL,
	CHATTING_WHISPER,

	//�ΰ��� �� Ŭ���̾�Ʈ ��û ��Ŷ ����
	REQ_CHARACTER_LIST,
	REQ_MONSTER_LIST,

	//�ɸ��� 
	ENTER_NEW_CHARACTER,
	ENTER_NEW_CHARACTER_VECTOR,

	LEAVE_CHARACTER,
	LEAVE_CHARACTER_VECTOR,

	//ĳ���� �̵�����
	MOVING,
	MOVING_VECTOR,

	//���� ����
	RESPAWN_MONSTER,
	RESPAWN_MONSTER_VEC,

	//���� �̵�
	MONSTER_MOVING_VECTOR,

	//���ݰ���
	ATTACK,
	MONSTER_ATTACK,
	REQ_DAMAGE,
	CON_DAMAGE,

	DIE,

	REQ_DAMAGE_VECTOR,
	CON_DAMAGE_VECTOR,

	//��Ż
	REQ_PORTAL,
	CON_PORTAL,

	//����
	


	TEST
};

typedef Packet_Symbal PS;

struct Ping
{
	int64_t time;
};
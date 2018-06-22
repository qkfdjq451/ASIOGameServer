#include "../../../Global.h"
#include "InGameState.h"

#include "../../../User/User.h"
#include "../../../User/UserManager.h"
#include "../../../Map/MapManager.h"
#include "../../../Map/MapInfo.h"
#include "../../../Character/CharacterManager.h"
#include "../../../Character/Character.h"
#include "../../../Channel/Channel.h"

#include"../../../DamageFormula/DamageFormula.h"

void InGameState::On_Read(const PS& symbol, void* recv_data, unsigned short size)
{
	
	switch (symbol)
	{
	case PS::CHATTING_ALL:
		{
			auto ss = session.lock();
			auto user = ss->GetUser().lock();
			if (ss != nullptr&&user != nullptr)
			{
				auto character = user->GetCharacter();
				if (!character) return;
				auto cm = character->GetCharacterManager().lock();
				if (!cm) return;

				auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
				auto charb = FB::ChatBuilder(*fbb);
				auto chat = FB::GetChat(recv_data);
				auto nick = fbb->CreateString(character->GetName());
				auto message = fbb->CreateString(chat->message());
				charb.add_nick(nick);
				charb.add_message(message);
				fbb->Finish(charb.Finish());
				cm->Async_SendAllCharacter(PS::CHATTING_ALL, fbb);
			}
		}
		break;
	case PS::REQ_ENTER_MAP:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();

		if (ss != nullptr&&user != nullptr)
		{
			auto map = FB::GetMap(recv_data);
			user->GetCharacter()->SetMapKey(map->mapcode());
		}
	}
		break;
	case PS::REQ_ENTER_CHANNEL:
	{
		auto ss = session.lock();		
		auto user = ss->GetUser().lock();

		if (ss != nullptr&&user != nullptr)
		{
			auto map = FB::GetMap(recv_data);
			user->GetCharacter()->SetChannel(map->channel());
		}
	}	
		break;
	case PS::MOVING:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();

		if (ss != nullptr&&user != nullptr)
		{
			auto moveInfo = FB::GetMove(recv_data);
			auto speed = moveInfo->speed();
			Vector3 positionVector = Vector3(moveInfo->position());
			Vector3 forwardVector = Vector3(moveInfo->foward());
			auto state = moveInfo->state();
			ComVector vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mapmanager = std::dynamic_pointer_cast<MapManager> (vec[0]);
			if (mapmanager)
			{
				auto character = user->GetCharacter();
				auto mapinfo = mapmanager->GetMapInfo(user->GetCharacter()->GetMapKey());
				auto channel = mapinfo->GetChannel(user->GetCharacter()->GetChannel());
				auto cm = channel->GetComponent<CharacterManager>();
				auto func = make_shared<Function<FB::MoveState>>([character, speed,forwardVector, positionVector](FB::MoveState state)
				{
					character->SetPositionZ(positionVector.z);
					character->SetDestination(positionVector);
					character->SetForward(forwardVector);
					character->SetCurrentSpeed(speed);
					state == FB::MoveState::MoveState_MOVING ? character->SetMove(true) : character->SetMove(false);
				}, state);
				cm->Async_Function(func);
			}
		}
	}
	break;
	case PS::ATTACK:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr&&user->GetCharacter()!=nullptr)
		{
			auto attack = FB::GetAttack(recv_data);
			auto channel = GetChannel(user->GetCharacter());
			if (channel)
			{
				auto cm = channel->GetComponent<CharacterManager>();
				auto fbb = make_shared<flatbuffers::FlatBufferBuilder>();
				auto attackb = FB::AttackBuilder(*fbb);
				attackb.add_code(attack->code());
				attackb.add_yaw(attack->yaw());
				attackb.add_state(attack->state());
				fbb->Finish(attackb.Finish());
				cm->Async_SendAllCharacter(PS::ATTACK, fbb);
			}
		}
	}
	break;
	case PS::REQ_DAMAGE_VECTOR:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
		{
			auto attack = FB::GetAttack(recv_data);
			auto channel = GetChannel(user->GetCharacter());
			if (channel)
			{
				auto cm = channel->GetComponent<CharacterManager>();
				auto fbb = make_shared<flatbuffers::FlatBufferBuilder>();
				auto attackb = FB::AttackBuilder(*fbb);
				attackb.add_code(attack->code());
				attackb.add_yaw(attack->yaw());
				attackb.add_state(attack->state());
				fbb->Finish(attackb.Finish());
				cm->Async_SendAllCharacter(PS::ATTACK, fbb);
			}
		}
	}
	break;

	case PS::PING_TEST:
	{
		auto ss = session.lock();
		ss->PushSend(PS::PING_TEST, recv_data, size);		

		break;
	}

	default:
		break;
	}
}

std::shared_ptr<class Channel> InGameState::GetChannel(std::shared_ptr<class Character> character)
{
	ComVector vec;
	Component::GetComponents_For_Tag("MapManager", vec);
	auto mapmanager = std::dynamic_pointer_cast<MapManager> (vec[0]);
	if (mapmanager)
	{
		auto mapinfo = mapmanager->GetMapInfo(character->GetMapKey());
		auto channel = mapinfo->GetChannel(character->GetChannel());
		return channel;
	}
	return nullptr;
}

void InGameState::ResDamageVector(void * recv_data)
{
	auto ss = session.lock();
	auto user = ss->GetUser().lock();
	if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
	{
		auto temp = FB::GetDamageVec(recv_data);
		auto dv = temp->damagevector();
		if (!dv)  return;
		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
		for (auto damage : *dv)
		{
			auto character = user->GetCharacter();
			if (character->GetCode() == damage->attacker_code())
			{
				float curDamage, temp;
				DamageFormula::Get()->GetCurDamage(character->GetCode(), damage->attackType(), 1, character->GetPower(), curDamage, temp);			
				auto DamageB = FB::DamageBuilder(*fbb);
				DamageB.add_attacker_code(character->GetCode());
				DamageB.add_attacker_type(FB::PlayerType::PlayerType_Player);
				DamageB.add_damaged_code(damage->damaged_code());
				DamageB.add_damaged_type(FB::PlayerType::PlayerType_Monster);
				
			}
		}
	}
}

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
#include "../../../Monster/MonsterManager.h"

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

				auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
				auto chat = FB::GetChat(recv_data);
				auto nick = fbb->CreateString(character->GetName());
				auto message = fbb->CreateString(chat->message());
				auto charb = FB::ChatBuilder(*fbb);
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
	case PS::REQ_CHARACTER_LIST:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
		{
			auto cm = user->GetCharacter()->GetCharacterManager().lock();
			if (!cm) return;
			auto func = std::make_shared<Function<void>>([ss,cm] 
			{
				vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>> fbbArray;
				int total = cm->Make_FBB_All_CharacterInfo(fbbArray);
				if (total != 0)
				{
					for (auto fbb : fbbArray)
					{
						ss->PushSend(PS::ENTER_NEW_CHARACTER_VECTOR, move(fbb));
					}
				}
			});
			cm->Async_Function(func);
		}
		break;
	}

	//case PS::REQ_MONSTER_LIST:
	//{
	//	auto ss = session.lock();
	//	auto user = ss->GetUser().lock();
	//	if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
	//	{
	//		//맵정보 얻어오기
	//		vector<std::shared_ptr<Component>> vec;
	//		Component::GetComponents_For_Tag("MapManager", vec);
	//		auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
	//		if (!mm) return;
	//		auto mapInfo = mm->GetMapInfo(user->GetCharacter()->GetMapKey());
	//		if (mapInfo != nullptr)
	//		{
	//			//채널 정보를 가져온다.
	//			auto channel = mapInfo->GetChannel(user->GetCharacter()->GetChannel());				
	//			//맵 타입이 Dungeon일 때 몬스터 정보를 보내준다.
	//			if (MapType::Dungeon == mapInfo->GetMapType())
	//			{
	//				auto monsterMng = channel->GetComponent<MonsterManager>();
	//				if (!monsterMng) return;
	//				auto func = std::make_shared<Function<void>>([ss, monsterMng]
	//				{
	//					auto fbb = monsterMng->Make_FBB_All_Monster();
	//					auto monsterVec = FB::GetMonsterVec(fbb->GetBufferPointer());
	//					auto mv = monsterVec->monvector();
	//					ss->PushSend(PS::RESPAWN_MONSTER_VEC, fbb);
	//				});
	//				monsterMng->Async_Function(func);
	//			}
	//		}
	//	}
	//	break;
	//}
	
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
				if (!channel)return;
				auto cm = channel->GetComponent<CharacterManager>();
				if (!cm)return;
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
	case PS::REQ_SKILL:
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
				cm->Async_SendAllCharacter(PS::CON_SKILL, fbb);
			}
		}
	}
	break;
	case PS::REQ_DAMAGE:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
		{
			shared_ptr<char[]> data(new char[size], std::default_delete<char[]>());
			memcpy(data.get(), recv_data, size);
			auto channel = GetChannel(user->GetCharacter());
			if (!channel) return;
			auto mm = channel->GetComponent<MonsterManager>();
			if (!mm) return;
			auto func = make_shared<Function<void>>
				(std::bind(&MonsterManager::DamageProcess, mm, data, user->GetCharacter()));
			mm->Async_Function(func);
		}
		break;
	}
	case PS::REQ_DAMAGE_VECTOR:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr&&user->GetCharacter() != nullptr)
		{
			shared_ptr<char[]> data(new char[size], std::default_delete<char[]>());			
			memcpy(data.get(), recv_data, size);
			auto channel = GetChannel(user->GetCharacter());
			if (!channel) return;
			auto mm = channel->GetComponent<MonsterManager>();
			if(!mm) return;
			auto func = make_shared<Function<void>>
				(std::bind(&MonsterManager::DamageProcessVector, mm, data, user->GetCharacter()));
			mm->Async_Function(func);
		}
	}
	break;
	case PS::REQ_PORTAL:
	{
		cout << "포탈 요청!" << endl;
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr && user->GetCharacter() != nullptr)
		{
			auto portalTable = FB::GetPortal(recv_data);
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (!mm) return;
			auto mapinfo = mm->GetMapInfo(user->GetCharacter()->GetMapKey());
			if (!mapinfo) return;
			auto portal = mapinfo->GetPortal(portalTable->gateNumber());
			if (!portal) return;
			if (portal->destCode != portalTable->currentMapCode())
			{
				user->GetCharacter()->SetWarpLocateDestination(portal->position);
				user->GetCharacter()->SetPosition(portal->position);
				user->GetCharacter()->SetMapKey(portal->destCode);
				//auto changed_mapinfo = mm->GetMapInfo(user->GetCharacter()->GetMapKey());
			}
			
			if (portalTable->destinationMapCode() == user->GetCharacter()->GetMapKey())
			{

			}
			//portalTable->currentMapCode()
		}
	}
	break;
	case PS::REQ_ESCAPE:
	{
		auto ss = session.lock();
		auto user = ss->GetUser().lock();
		if (ss != nullptr&&user != nullptr && user->GetCharacter() != nullptr)
		{
			auto cm = user->GetCharacter()->GetCharacterManager().lock();
			if (!cm) return;
			auto channel =cm->GetParentComponent<Channel>();
			if (!channel) return;
			channel->Async_Func(bind(&Channel::Escape, channel, user->GetCharacter()));
		}

		break;
	}
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


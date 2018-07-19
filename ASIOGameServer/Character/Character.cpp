#include "../Global.h"
#include "Character.h"

#include "../Map/MapInfo.h"
#include "../Map/MapManager.h"
#include "../Channel/Channel.h"
#include "../User/User.h"
#include "../Asio/Session/Session.h"
#include "../Character/CharacterManager.h"

#include "../Navi/Navigation.h"

#include "../ItemManager/Inventory.h"

#include "../StatusManager/StatusManager.h"

Character::Character(std::shared_ptr<class User> user, int code)
	:user(user), code(code) , forward(0,0,0), bMove(false), bChangePosition(false)
{	
	mapKey.Set(0);
	channel.Set(0);
	inventory = make_shared<Inventory>();
}

Character::~Character()
{
	cout << "케릭터 사라짐" << endl;
}

void Character::Init()
{
	auto self = shared_from_this();

	//맵, 채널 이동 처리
	mapKey.InsertPrevEvent([self,this](int mapKey)
	{		
		channel.Set(0);
	});

	mapKey.InsertPostEvent([this,self](int mapKey)
	{
		if (mapKey != 0)
		{
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (mm)
			{
				auto mapinfo = mm->GetMapInfo(mapKey);
				auto shareduser = user.lock();
				if (shareduser)
				{
					auto sesstion = shareduser->GetSesstion().lock();
					if (sesstion)
					{
						auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
						auto levelName = fbb->CreateString(mapinfo->GetLevelName());
						auto mapB = FB::MapBuilder(*fbb);						
						mapB.add_mapcode(mapKey);
						mapB.add_mapLevelName(levelName);		
						mapB.add_position(&warpLocateDestination.ToFBVector3());
						fbb->Finish(mapB.Finish());

						//auto zz = mapinfo->GetLevelName();

						sesstion->PushSend(PS::CON_ENTER_MAP, fbb);
					}
				}
				mapinfo->InsertCharacter(self);
			}
		}

		//TODO : 나중에 1->code 로 변경
		//inventory->SetInventory(code);
		//inventory->SetInventory(1);
	});

	channel.InsertPrevEvent([this](int channel)
	{
		if (channel != 0)
		{
			//채널이 변경됐을 때 이전에 해야할 처리
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (mm)
			{
				auto mapinfo = mm->GetMapInfo(mapKey.Get());
				mapinfo->GetChannel(channel)->Async_EraseCharacter(code);				
			}
		}
	});

	channel.InsertPostEvent([self, this](int key)
	{
		//채널이 변경된 이후에 해야할 처리
		if (key != 0)
		{
			vector<std::shared_ptr<Component>> vec;
			Component::GetComponents_For_Tag("MapManager", vec);
			auto mm = dynamic_pointer_cast<MapManager>(vec[0]);
			if (mm)
			{
				auto mapinfo = mm->GetMapInfo(mapKey.Get());
				auto channel = mapinfo->GetChannel(key); 
				channel->Async_InsertCharacter(self);
			}
		}

	});
}

void Character::Clear()
{
	mapKey.ClearEvent();
	channel.ClearEvent();
}

void Character::SetForward(Vector3 _forward)
{
	_forward.Normalize();
	forward = _forward;	
}

void Character::Moving(float delta)
{
	if (bMove)
	{
		if (currentSpeed > maxSpeed)
			currentSpeed = maxSpeed;
	}
	else
	{
		//if (currentSpeed < 0)
		if (Vector3::Distance(dest, position)<10)
		{
			currentSpeed = maxSpeed;
			SetForward(dest - position);
		}
		else
		{
			currentSpeed = 0;
		}
	}
	if (currentSpeed != 0)
	{
		bChangePosition = true;		
		position += forward * delta * currentSpeed;
	}
	
}

void Character::GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>>& vec)
{
	if (bChangePosition)
	{
		auto moveb=FB::MoveBuilder(*fbb);
		moveb.add_code(code);
		bMove ? moveb.add_state(FB::MoveState::MoveState_MOVING): moveb.add_state(FB::MoveState::MoveState_STOP);
		moveb.add_foward(&forward.ToFBVector3());
		moveb.add_position(&position.ToFBVector3());
		moveb.add_speed(currentSpeed);
		vec.push_back(moveb.Finish());
		bChangePosition = false;
	}
}

bool Character::LevelUp()
{
	currentExp -= reqExp;
	StatusManager::Get()->SetStatus(shared_from_this());
	return true;
}

void Character::AddCurrentExp(int _value)
{
	currentExp += _value;
	if (currentExp >= reqExp)
	{
		LevelUp();
	}
}


#include "../Global.h"
#include "Character.h"

#include "../Map/MapInfo.h"
#include "../Map/MapManager.h"
#include "../Channel/Channel.h"
#include "../User/User.h"
#include "../Asio/Session/Session.h"

#include "../Navi/Navigation.h"

Character::Character(std::shared_ptr<class User> user, int code)
	:user(user), code(code) , forward(0,0,0), bMove(false), bChangePosition(false)
{	
	mapKey.Set(0);
	channel.Set(0);
}

Character::~Character()
{
	cout << "케릭터 사라짐" << endl;
}

void Character::Init()
{
	auto self = shared_from_this();

	mapKey.InsertPrevEvent([self,this](int mapKey)
	{		
		channel.Set(0);
	});

	mapKey.InsertPostEvent([this,self](int mapKey)
	{
		cout << "멥키 : " << mapKey << endl;
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
						sesstion->PushSend(PS::CON_ENTER_MAP);
					}
				}
				mapinfo->InsertCharacter(self);
			}
		}

	});

	channel.InsertPrevEvent([this](int channel)
	{
		if (channel != 0)
		{
			//TODO : 채널이 변경됐을 때 이전에 해야할 처리
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
		//TODO : 채널이 변경된 이후에 해야할 처리
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


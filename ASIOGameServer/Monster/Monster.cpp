#include "../Global.h"
#include "../Navi/Navigation.h"
#include "Monster.h"

Monster::Monster(int code)
:code(code)
{
}

Monster::~Monster()
{
}

void Monster::BeginPlay()
{
	bAlive.InsertPostEvent([this](bool val)
	{
		if (val)
		{
			Respawn();
		}
		else
		{

		}
	});
	bAlive.Set(true);
	bMove = false;
}

void Monster::PrevTick()
{
}

void Monster::Tick()
{
	PrevTick();
}

void Monster::EndPlay()
{
}

void Monster::Respawn()
{
	auto nv = navi.lock();
	if (nv)
	{
		dtQueryFilter filter;
		filter.setIncludeFlags(SamplePolyFlags::SAMPLE_POLYFLAGS_WALK);
		nv->GetQuery()->findRandomPoint(&filter, Navigation::frand, &m_startRef, (float*)&position);
		position.ToUnrealLocation();

		position *= 100;
		cout << position.ToString() << endl;
	}
	else
	{	
		uniform_real_distribution<float> rangeX(respawn_range.minX, respawn_range.maxX);
		uniform_real_distribution<float> rangeY(respawn_range.minY, respawn_range.maxY);
		Vector3 pos;
		pos.x = rangeX(Navigation::rnd);
		pos.y = rangeY(Navigation::rnd);

		position = pos;
	}	
}

void Monster::SetForward(Vector3 _forward)
{
	_forward.Normalize();
	forward = _forward;	
}

void Monster::Moving(float delta)
{
	if (bMove)
	{
		bChangePosition = true;
		position += forward * delta * speed;
	}
}

void Monster::GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>>& vec)
{
	if (bChangePosition)
	{
		auto moveb=FB::MoveBuilder(*fbb);
		moveb.add_code(code);
		bMove ? moveb.add_state(FB::MoveState::MoveState_MOVING): moveb.add_state(FB::MoveState::MoveState_STOP);
		moveb.add_foward(&forward.ToFBVector3());
		moveb.add_position(&position.ToFBVector3());
		moveb.add_speed(speed);
		vec.push_back(moveb.Finish());
		bChangePosition = false;
	}
}


#include "../Global.h"
#include "../Navi/Navigation.h"
#include "Monster.h"
#include "../Channel/Channel.h"
#include "../Character/CharacterManager.h"
#include "../Character/Character.h"
#include "../BehaviorTree/BehaviorTree.h"
#include "../DamageFormula/DamageFormula.h"

Monster::Monster(int code)
:code(code) , ai(nullptr)
{
	bMove = false;
	bChangePosition = false;
	state = IDLE;
	idleState = HOLD;
	respawnTime=0;
	curRespawnTime=0;
	curIdleTime = 0;	
	idleTime = 5;
	attackCoolTime = std::chrono::system_clock::now();
}

Monster::~Monster()
{
	if (ai)
	{
		delete ai;
		ai = nullptr;
	}
}

void Monster::BeginPlay()
{
	bAlive.InsertPostEvent([this](bool val)
	{
		//살아 났을 때
		if (val)
		{
			Respawn();
		}
		//죽었을 때
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
	if (!ai)return;
	ai->Run(Time::Delta());
}

void Monster::EndPlay()
{
}

void Monster::Respawn()
{
	auto nv = navi.lock();
	if (nv)
	{
		Vector3 spawnPosition;
		nv->GetQuery()->findRandomPoint(&Navigation::GetQueryFilter(), Navigation::frand, &m_startRef, (float*)&spawnPosition);
		position = spawnPosition.ToUnrealLocation();

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

bool Monster::GetDamage(std::shared_ptr<class Character> attacker, FB::AttackState state)
{
	DamageFormula::Get()->CheckRange(attacker->Get(), state,)
	return false;
}


void Monster::SetForward(Vector3 _forward)
{
	_forward.Normalize();
	forward = _forward;	
}

bool Monster::Moving(float delta)
{
	if (bMove)
	{
		bChangePosition = true;
		position += forward * delta * speed;
		return true;
	}
	return false;
}

void Monster::GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>>& vec)
{
	if (bChangePosition)
	{
		auto moveb=FB::MoveBuilder(*fbb);
		moveb.add_code(monsterNumber);
		bMove ? moveb.add_state(FB::MoveState::MoveState_MOVING): moveb.add_state(FB::MoveState::MoveState_STOP);
		moveb.add_foward(&forward.ToFBVector3());
		moveb.add_position(&position.ToFBVector3());
		moveb.add_speed(speed);
		vec.push_back(moveb.Finish());
		bChangePosition = false;
	}
}

bool Monster::FindPath(Vector3 dest)
{
	auto nav = navi.lock();
	if (nav)
	{
		unsigned int path[256];
		int pathCount=0;

		auto query = nav->GetQuery();
		Vector3 destination = dest;
		destination.ToGLLocation();
		Vector3 glPos = position;
		glPos.ToGLLocation();

		Vector3 straightPath[256];
		int straightPathCount=0;
		//unsigned char m_straightPathFlags[256];
		//dtPolyRef m_straightPathPolys[256];
		
		//시작점 찾기
		query->findNearestPoly((float*)&glPos,
			(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_startRef, 0);
		//끝나는 점 찾기
		query->findNearestPoly((float*)&destination,
			(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_endRef, 0);


		query->findPath(m_startRef, m_endRef,
			(float*)&glPos, (float*)&destination, &Navigation::GetQueryFilter(), path, &pathCount, 256);

		auto result= query->findStraightPath((float*)&glPos, (float*)&destination, path, pathCount, 
			(float*)straightPath, 0, 0, &straightPathCount, 256, DT_STRAIGHTPATH_ALL_CROSSINGS);

		for (int i = 0; i < straightPathCount; i++)
		{
			paths.push_back(straightPath[i].ToUnrealLocation());
		}


	/*	query->findPath(m_startRef, m_endRef,
			(float*)&glPos, (float*)&destination, &Navigation::GetQueryFilter(), path, &pathCount, 256);
		paths.push_back(glPos);*/

		/*Vector3 closest;
		query->closestPointOnPoly(m_startRef, (float*)&paths.back(), (float*)&closest, 0);
		for (int i = 1; i < pathCount; i++)
		{
			query->closestPointOnPoly(path[i], (float*)&paths.back(), (float*)&closest, 0);
			paths.back().ToUnrealLocation();
			paths.push_back(closest);
		}

		paths.back().ToUnrealLocation();*/
		//paths.pop_front();
		

		return true;
	}
}


bool Monster::UpdateState(float delta)
{
	if (bAlive.Get() == false)
	{
		state = State::DIE;
		return true;
	}

	auto channel = GetParentComponent<Channel>();
	if (!channel) return false;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return false;
	auto result = cm->SearchNearPlayer(position);
	if (result.first != -1 && result.second < 1000)
	{
		if (result.second > 50)
		{
			state = State::CHASE;
			paths.clear();
			bMove = true;
			targetCode = result.first;
			auto character = cm->GetCharacter(targetCode);
			if (!character) return true;
			FindPath(character->GetPosition());
		/*	cout << "캐릭터 위치 : "<<character->GetPosition().ToString() << endl;			
			cout << "몬스터 타겟 : "<<paths.back().ToString() << endl;
			cout << "몬스터 위치 : " << position.ToString() << endl;
			cout << "몬스터 길 갯수 : " << paths.size() << endl;*/
			return true;
		}
		else
		{
			state = State::ATTACK;
			bMove = false;
			targetCode = result.first;
		}
	}
	else
	{
		state = State::IDLE;
	}
	//destination = character->GetPosition();
	return true;
	
}

bool Monster::UpdateIdle(float delta)
{
	switch (idleState)
	{
	case Monster::PATROL:
	{
		if (paths.empty())
		{
			curIdleTime = 0;
			uniform_real_distribution<float> idleTimeRange(3, 5);
			idleTime = idleTimeRange(Navigation::rnd);
			idleState = HOLD;
			bMove = true;
		}
		break;
	}
	case Monster::HOLD:
	{
		if (idleTime > curIdleTime) return true;
		bMove = true;
		auto nv = navi.lock();
		if (nv)
		{
			auto query = nv->GetQuery();
			Vector3 start = position, destination;
			start.ToGLLocation();
			
			query->findNearestPoly((float*)&start,
				(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_startRef, 0);

			query->findRandomPointAroundCircle(m_startRef,(float*)&start,15,
				&Navigation::GetQueryFilter(), Navigation::frand,
				&m_endRef, (float*)&destination);
			destination.ToUnrealLocation();
			FindPath(destination);
			
			idleState = PATROL;
		}
	}
	}
	return true;
}

bool Monster::Patrol(float delta)
{
	if (paths.empty()) return true;
	if (Vector3::Distance(position, paths.front()) < 5)
		paths.pop_front();
	if (paths.empty()) return true;
	SetForward(paths.front() - position);
	Moving(delta);
	return true;
}

bool Monster::Hold(float delta)
{
	curIdleTime += delta;
	return false;
}

bool Monster::Chase(float delta)
{
	if (paths.empty()) return true;
	if (Vector3::Distance(position, paths.front()) < 5)
		paths.pop_front();
	if (paths.empty()) return true;
	SetForward(paths.front() - position);
	Moving(delta);
	return false;
}

bool Monster::Attack(float delta)
{
	std::chrono::duration<float> sec = attackCoolTime - std::chrono::system_clock::now();
	if (sec.count() < 1000) 	return true;	
	attackCoolTime = std::chrono::system_clock::now();
	auto channel = GetParentComponent<Channel>();
	if (!channel) return false;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return false;
	//auto fbb = shared_ptr<flatbuffers::FlatBufferBuilder>();
	//auto attack
	//cm->Async_SendAllCharacter();
	
	return false;
}


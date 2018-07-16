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
	bAttacking = false;
	state = IDLE;
	idleState = HOLD;
	curRespawnTime=0;
	curIdleTime = 0;	
	idleTime = 5;	
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
	auto self = shared_from_this();
	bAlive.InsertPostEvent([self, this](bool val)
	{
		//살아 났을 때
		if (val)
		{
			Respawn();
		}
		//죽었을 때
		else
		{
			Dead();
			cout << "죽어랏!" << endl;
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
	bAlive.ClearEvent();
}

void Monster::Respawn()
{
	curRespawnTime = 0;
	currentHp = maxHp;
	state = IDLE;
	bAttacking = false;
	auto nv = navi.lock();
	if (nv)
	{
		Vector3 start = respawnPosition;
		Vector3 spawnPosition;
		start.ToGLLocation();

		nv->GetQuery()->findNearestPoly((float*)&start,
			(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_startRef, 0);

		nv->GetQuery()->findRandomPointAroundCircle(m_startRef, (float*)&start, respawnRange/100.f,
			&Navigation::GetQueryFilter(), Navigation::frand,
			&m_endRef, (float*)&spawnPosition);

		position = spawnPosition.ToUnrealLocation();
	}
	
	auto channel = GetParentComponent<Channel>();
	if (!channel) return;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return;
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
	auto nick = fbb->CreateString(nickname);
	auto monsterB = FB::MonsterBuilder(*fbb);
	SetMonsterBuilder(&monsterB);	
	monsterB.add_nick(nick);
	fbb->Finish(monsterB.Finish());
	cm->Async_SendAllCharacter(PS::RESPAWN_MONSTER, fbb);
}

void Monster::Dead()
{
	auto channel = GetParentComponent<Channel>();
	if (!channel) return;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return;
	
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
	fbb->Finish(FB::CreateDie(*fbb, monsterNumber, FB::PlayerType::PlayerType_Monster));
	cm->Async_SendAllCharacter(PS::DIE, fbb);
}

void Monster::SetMonsterBuilder(FB::MonsterBuilder * mb)
{
	mb->add_code(code);
	mb->add_CurrentHP(currentHp);
	mb->add_MaxHp(maxHp);
	mb->add_number(monsterNumber);
	mb->add_position(&position.ToFBVector3());
	mb->add_power(power);
	mb->add_speed(speed);
}

bool Monster::CheckRange(std::shared_ptr<class Character> attacker, FB::AttackState state)
{
	if (DamageFormula::Get()->CheckRange(attacker->GetTypeCode(), state, attacker->GetPosition(), position) == false)
		return false;
	return true;
}

bool Monster::GetDamage(std::shared_ptr<class Character> attacker, FB::AttackState state, FB::DamageBuilder * db)
{
	if (bAlive.Get() == false)
		return false;
	float temp, damageval;
	if (DamageFormula::Get()->GetCurDamage(attacker->GetTypeCode(), state, 1, attacker->GetPower(), damageval, temp)==false)
		return false;
	currentHp -= damageval;
	if (currentHp <= 0)
		bAlive.Set(false);
	if (db)
	{
		db->add_attacker_code(attacker->GetCode());
		db->add_attacker_type(FB::PlayerType::PlayerType_Player);
		db->add_attackType(state);
		db->add_currentHP(currentHp);
		db->add_damage(damageval);
		db->add_damaged_type(FB::PlayerType::PlayerType_Monster);
		db->add_damaged_code(monsterNumber);
	}
	return true;
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
		dtPolyRef straightPathPolys[256];
		
		//시작점 찾기
		query->findNearestPoly((float*)&glPos,
			(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_startRef, 0);
		//끝나는 점 찾기
		query->findNearestPoly((float*)&destination,
			(float*)&Navigation::GetPolyPickExt(), &Navigation::GetQueryFilter(), &m_endRef, 0);


		query->findPath(m_startRef, m_endRef,
			(float*)&glPos, (float*)&destination, &Navigation::GetQueryFilter(), path, &pathCount, 256);

		

		auto result= query->findStraightPath((float*)&glPos, (float*)&destination, path, pathCount, 
			(float*)straightPath, 0, straightPathPolys, &straightPathCount, 256, DT_STRAIGHTPATH_ALL_CROSSINGS);

		for (int i = 0; i < straightPathCount; i++)
		{
			float height;
			query->getPolyHeight(straightPathPolys[i], (float*)&straightPath[i], &height);
			straightPath[i].y = height;
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
	return false;
}


bool Monster::UpdateState(float delta)
{
	if (bAlive.Get() == false)
	{
		state = State::DIE;
		if (curRespawnTime > respawnTime)
			bAlive.Set(true);
		return true;
	}

	if (bAttacking)
	{
		state = State::ATTACK;
		bMove = false;
	}

	auto channel = GetParentComponent<Channel>();
	if (!channel) return false;
	auto cm = channel->GetComponent<CharacterManager>();
	if (!cm) return false;
	auto result = cm->SearchNearPlayer(position);
	if (result.first != -1 && result.second < 1000)
	{
		if (result.second > 100)
		{
			state = State::CHASE;
			//cout << "타겟 케릭터 코드 : " << result.first << endl;
			//cout << "타겟 케릭터 거리 : " << result.second << endl;
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
	if (bAttacking==true)
	{
		std::chrono::duration<float> sec =std::chrono::system_clock::now()- attackCoolTime;
		if (sec.count() < 1.7)
		{
			return true;
		}
		else
		{
			bAttacking = false;		
			return true;
		}
	}
	else
	{
		bAttacking = true;
		attackCoolTime = std::chrono::system_clock::now();
		auto channel = GetParentComponent<Channel>();
		if (!channel) return false;
		auto cm = channel->GetComponent<CharacterManager>();
		if (!cm) return false;
		//TODO : 플레이어들에게 몬스터가 공격을 시작했 다는 것을 보내주기
		auto fbb = make_shared<flatbuffers::FlatBufferBuilder>();
		auto attackB = FB::AttackBuilder(*fbb);
		attackB.add_code(monsterNumber);
		attackB.add_state(FB::AttackState::AttackState_Combo1);
		attackB.add_targetCode(targetCode);
		fbb->Finish(attackB.Finish());
		cm->Async_SendAllCharacter(PS::MONSTER_ATTACK, fbb);
	}

	return false;
}

bool Monster::Die(float delta)
{
	curRespawnTime += delta;
	return false;
}


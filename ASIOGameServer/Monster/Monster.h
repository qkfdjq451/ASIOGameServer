#pragma once

class Monster :public Component
{
public:
	enum State : int
	{
		IDLE = 0,
		CHASE,
		ATTACK,
		DIE,
	};

	enum IdleState : int
	{
		PATROL,
		HOLD,
	};

	friend class MonsterManager;
	friend class MonsterBuilder;

	Monster(int code);
	~Monster();



public:
	//접근자
	//Get
	int GetCode() { return code; }
	int GetNumber() { return monsterNumber; }
	std::string GetName() { return nickname; }
	float GetMaxHP() { return maxHp; }
	float GetCurrentHp() { return currentHp; }
	float GetPower() { return power; }
	float GetSpeed() { return speed; }
	Vector3 GetPosition() { return position; }
	
	//Set
	void SetNumber(int number) { monsterNumber = number; }
	void SetName(std::string _name) { nickname = _name; }
	void SetMaxHP(float _hp) { maxHp = _hp; }
	void SetCurrentHp(float _hp) { currentHp =(_hp); }
	void SetPower(float _power) { power=_power; }
	void SetSpeed(float _speed) { speed = _speed; }
	void SetPosition(const Vector3& vec3) { position = vec3; }
	
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;


	//기능
	void Respawn();
	void Dead();
	void SetMonsterBuilder(FB::MonsterBuilder* mb);
	bool CheckRange(std::shared_ptr<class Character> attacker, FB::AttackState state);
	bool GetDamage(std::shared_ptr<class Character> attacker, FB::AttackState state, FB::DamageBuilder * db = nullptr);


	//이동 관련 변수와 함수
	bool bMove;
	bool bChangePosition;
	Vector3 forward;
	void SetMove(bool move) { bMove = move; }
	void SetForward(Vector3 _forward);
	bool Moving(float delta);
	void GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>> &vec);


	//Monster 행동에 관한 함수
	bool FindPath(Vector3 dest);	
	bool UpdateState(float delta);

	bool UpdateIdle(float delta);
	bool Patrol(float delta);
	bool Hold(float delta);

	bool Chase(float delta);
	bool Attack(float delta);
	bool Die(float delta);

	//행동에 관한 변수
	class BTNode* ai;
	State state;
	int targetCode;
	IdleState idleState;
	float idleTime;
	float curIdleTime;

	bool bAttacking;
	std::chrono::system_clock::time_point attackCoolTime;

	std::deque<Vector3> paths;
	

protected:
	int code;					//몬스터 키
	int monsterNumber;		//몬스터 식별 번호

	std::string nickname;

	//생존, 채력, 리스폰
	React<bool> bAlive;
	float maxHp;
	float currentHp;
	float respawnTime;
	float curRespawnTime;

	float respawnRange;
	Vector3 respawnPosition;


	float power;
	float speed;
	Vector3 position;


	std::weak_ptr<class Navigation> navi;
	unsigned int m_startRef;
	unsigned int m_endRef;
};
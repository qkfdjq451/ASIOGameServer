#pragma once

class Monster :public Component
{
public:
	friend class MonsterManager;

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
	float GetPower() { return power.Get(); }
	float GetSpeed() { return speed; }
	Vector3 GetPosition() { return position; }
	
	//Set
	void SetNumber(int number) { monsterNumber = number; }
	void SetName(std::string _name) { nickname = _name; }
	void SetMaxHP(float _hp) { maxHp = _hp; }
	void SetCurrentHp(float _hp) { currentHp =(_hp); }
	void SetPower(float _power) { power.Set(_power); }
	void SetSpeed(float _speed) { speed = _speed; }
	void SetPosition(const Vector3& vec3) { position = vec3; }
	
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;


	//기능
	void Respawn();
	void GetDamage(float Damage);


	//이동 관련 변수와 함수
	bool bMove;
	bool bChangePosition;
	Vector3 forward;
	void SetMove(bool move) { bMove = move; }
	void SetForward(Vector3 _forward);
	void Moving(float delta);
	void GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>> &vec);


	RespawnRange& GetRespawnRange() { return respawn_range; }
protected:
	int code;					//몬스터 키
	int monsterNumber;		//몬스터 식별 번호

	std::string nickname;
	RespawnRange respawn_range;

	React<bool> bAlive;
	float maxHp;
	float currentHp;

	React<float> power;
	float speed;
	Vector3 position;


	std::weak_ptr<class Navigation> navi;
	unsigned int m_startRef;
	unsigned int m_endRef;
};
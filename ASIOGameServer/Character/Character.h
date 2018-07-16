#pragma once

class Character :public enable_shared_from_this<Character>
{
public:
	friend class CharacterManager;

	Character(std::shared_ptr<class User> user,int code);
	~Character();
	void Init();
	void Clear();

public:
	//접근자
	//Get
	int GetCode() { return code; }
	FB::CharacterType GetTypeCode() { return characterTypeCode; }
	int GetLevel() { return level; }
	std::string GetName() { return nickname; }
	float GetMaxHP() { return maxHP; }
	float GetCurrentHP() { return currentHP; }
	float GetPower() { return power; }
	float GetDefense() { return defense; }
	float GetCurrentSpeed() { return currentSpeed; }
	float GetSpeed() { return maxSpeed; }
	Vector3 GetPosition() { return position; }
	int GetChannel() { return channel.Get(); }
	int GetMapKey() { return mapKey.Get(); }

	
	//Set
	void SetTypeCode(FB::CharacterType _code) { characterTypeCode = _code; }
	void SetLevel(int _level) { level = _level; }
	void SetMapKey(int mapkey) { mapKey.Set(mapkey); }
	void SetChannel(int number) { channel.Set(number); }
	void SetName(std::string _name) { nickname = _name; }
	
	void SetMaxHP(float _hp) { maxHP = _hp; }
	void SetCurrentHP(float _hp) { currentHP = _hp; }

	void SetPower(float _power) { power = _power; }
	void SetDefense(float _defense) { defense = _defense; }

	void SetCurrentSpeed(float _speed) { currentSpeed = _speed; }
	void SetSpeed(float _speed) { maxSpeed = _speed; }
	void SetPosition(const Vector3& vec3) { position = vec3; }
	void SetPositionZ(float z) { position.z = z; }
	std::weak_ptr<class User> GetUserPointer() { return user; }

	//이동 관련 변수와 함수
	bool bMove;
	bool bChangePosition;
	Vector3 forward;
	void SetMove(bool move) { bMove = move; }
	void SetForward(Vector3 _forward);
	void SetDestination(Vector3 _pos) { dest = _pos; }
	void Moving(float delta);
	void GetMoveInfo(std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb, vector<flatbuffers::Offset<FB::Move>> &vec);

	std::weak_ptr<class CharacterManager> GetCharacterManager() { return cm; }

	void SetWarpLocateDestination(Vector3 _pos) { warpLocateDestination = _pos; }
	void SetWarpPosition() { warpLocateDestination = position; }

protected:
	int code;
	enum FB::CharacterType characterTypeCode;

	//자기 자신의  user와 manager
	std::weak_ptr<class User> user;
	std::weak_ptr<class CharacterManager> cm;

	//status
	int level;
	std::string nickname;
	
	float currentHP;
	float maxHP;

	float defense;
	float power;

	float maxSpeed;
	float currentSpeed;

	//맵과 좌표
	Vector3 position;
	Vector3 dest;

	React<int> mapKey;
	React<int> channel;
	
	Vector3 warpLocateDestination;

	//아이템 관련
	std::shared_ptr<class Inventory> inventory;
};
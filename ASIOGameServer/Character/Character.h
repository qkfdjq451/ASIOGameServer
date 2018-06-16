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
	int GetLevel() { return level; }
	std::string GetName() { return nickname; }
	float GetHP() { return hp.Get(); }
	float GetPower() { return power.Get(); }
	float GetCurrentSpeed() { return currentSpeed; }
	float GetSpeed() { return maxSpeed; }
	Vector3 GetPosition() { return position; }
	int GetChannel() { return channel.Get(); }
	int GetMapKey() { return mapKey.Get(); }
	

	//Set
	void SetLevel(int _level) { level = _level; }
	void SetMapKey(int mapkey) { mapKey.Set(mapkey); }
	void SetChannel(int number) { channel.Set(number); }
	void SetName(std::string _name) { nickname = _name; }


	void SetHP(float _hp) { hp.Set(_hp); }
	void SetPower(float _power) { power.Set(_power); }

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

protected:
	int code;

	std::weak_ptr<class User> user;

	int level;
	std::string nickname;
	

	React<float> hp;
	React<float> power;

	float maxSpeed;
	float currentSpeed;


	Vector3 position;
	Vector3 dest;

	React<int> mapKey;
	React<int> channel;


	std::weak_ptr<class Navigation> navi;
};
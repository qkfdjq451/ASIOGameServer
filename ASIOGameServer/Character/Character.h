#pragma once

class Character :public enable_shared_from_this<Character>
{
public:
	friend class CharacterManager;

	Character(std::shared_ptr<class User> user);
	~Character();
	void Init();

public:
	//Á¢±ÙÀÚ
	//Get
	int GetCode() { return code; }
	int GetLevel() { return level; }
	std::string GetName() { return nickname; }
	float GetHP() { return hp.Get(); }
	float GetPower() { return power.Get(); }
	float GetSpeed() { return speed; }
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

	void SetSpeed(float _speed) { speed = _speed; }
	void SetPosition(const Vector3& vec3) { position = vec3; }
	std::weak_ptr<class User> GetUserPointer() { return user; }



protected:
	int code;

	std::weak_ptr<class User> user;

	int level;
	std::string nickname;
	

	React<float> hp;
	React<float> power;

	float speed;

	Vector3 position;

	React<int> mapKey;
	React<int> channel;


	
};
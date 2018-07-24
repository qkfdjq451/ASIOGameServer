#pragma once
#include "../Global.h"

class User :public std::enable_shared_from_this<User>
{
public:
	friend class UserManager;
	User();
	~User();
public:	
	
	std::weak_ptr<class Session> GetSesstion() { return session; }
	void SetSession(std::shared_ptr<class Session> session);
	
	//계정 정보
public:
	int GetCodeKey() { return key; }

private:
	int key;

	std::string id;
	std::weak_ptr<class Session> session;

public:
	bool ClearCharacter();
	std::shared_ptr<class Character> LoadCharacterInfo(int characterCode);
	std::shared_ptr<class Character> GetCharacter() { return character; }	
private:
	std::shared_ptr<class Character> character;
	
};

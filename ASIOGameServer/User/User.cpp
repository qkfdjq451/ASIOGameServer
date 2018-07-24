
#include "User.h"
#include "../Character/Character.h"

#include "../Asio/Session/Session.h"
#include "../StatusManager/StatusManager.h"

User::User()
	:character(nullptr)
{
	

}

User::~User()
{
	if (character)
	{
		character->SetMapKey(0);
		character->SetChannel(0);
		character->Clear();
	}
}

void User::SetSession(std::shared_ptr<class Session> _session)
{
	session = _session;
}

bool User::ClearCharacter()
{
	return false;
}

std::shared_ptr<class Character> User::LoadCharacterInfo(int characterCode)
{
	string query = "Call GetCharacterInfo(" + to_string(characterCode) + ")";
	auto vec = MySQLManager::Get()->Query_Vector(query.c_str());

	character = std::make_shared<Character>(shared_from_this(), characterCode);
	character->SetTypeCode((FB::CharacterType)stoi(vec[0][2].second));

	auto type = character->GetTypeCode();
	character->SetName(vec[0][3].second);
	character->SetLevel(stoi(vec[0][4].second));
	character->SetCurrentExp(stoi(vec[0][5].second));
	cout << character->GetName() << endl;

	StatusManager::Get()->SetStatus(character);
	character->SetMapKey(stoi(vec[0][6].second));
	character->SetStartPoint(stoi(vec[0][7].second));
	auto position = Vector3(vec[0][8].second, vec[0][9].second, vec[0][10].second);
	character->SetWarpLocateDestination(position);
	character->SetPosition(position);

	character->Init();

	return character;
}

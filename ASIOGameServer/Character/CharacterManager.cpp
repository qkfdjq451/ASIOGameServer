#include "../Global.h"

#include "CharacterManager.h"
#include "Character.h"

#include "../Asio/Session/Session.h"

#include "../Map/MapInfo.h"

#include "../User/User.h"


void CharacterManager::Async_InsertCharacter(std::shared_ptr<class Character> character)
{
	async(std::launch::async, [this, character]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_Insert_list.push_back(character);
	});
}

void CharacterManager::Async_EraseCharacter(int key)
{
	async(std::launch::async, [this,key]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_erase_list.push_back(key);
	});
}

void CharacterManager::Async_SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
{
	async(std::launch::async, [this, symbol, fbb]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_send_all.push_back(make_pair(symbol,fbb));
	});
}

int CharacterManager::GetCharacterCount()
{
	return characters.size();
}

bool CharacterManager::InsertCharacter(std::shared_ptr<class Character> character)
{
	auto result = characters.find(character->code);
	if (result == characters.end())
	{
		characters.insert(std::make_pair(character->code, character));
		return true;
	}
	return false;
}

void CharacterManager::SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
{
	auto sb = std::make_shared<SendBuffer>(symbol, move(fbb));
	for (auto iter = characters.begin(); iter!= characters.end();++iter)
	{
		auto ch = iter->second.lock();
		if (ch)
		{
			auto user = ch->GetUserPointer().lock();
			if (user)
			{
				auto session = user->GetSesstion().lock();
				if (session)
				{
					session->PushSend(sb);
				}
			}
		}
		else
		{
			
		}
	}
}

std::shared_ptr<flatbuffers::FlatBufferBuilder> CharacterManager::Make_FBB_All_CharacterInfo()
{
	auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
	vector<flatbuffers::Offset<FB::Character>> vec;
	for (auto character : characters)
	{
		auto ch = character.second.lock();
		if (ch)
		{
			auto nick = fbb->CreateString(ch->GetName());
			auto charB = FB::CharacterBuilder(*fbb);
			charB.add_code(ch->GetCode());
			charB.add_hp(ch->GetHP());
			charB.add_level(ch->GetLevel());
			charB.add_nick(nick);
			charB.add_power(ch->GetPower());
			charB.add_speed(ch->GetSpeed());
			vec.push_back(charB.Finish());
		}
	}
	auto vecoffset = fbb->CreateVector(vec);
	fbb->Finish(FB::CreateCharacterVec(*fbb, vecoffset));
	return fbb;
}

bool CharacterManager::EraseCharacter(int key)
{
	auto result = characters.find(key);
	if (result != characters.end())
	{
		characters.erase(key);
		return true;
	}
	return false;
}

bool CharacterManager::Moving(std::shared_ptr<class Character> character)
{
	return false;
}

void CharacterManager::BeginPlay()
{
	for (auto character : characters)
	{
		auto ch = character.second.lock();
		if (ch)
		{
			
		}
	}
}

void CharacterManager::PrevTick()
{
	std::vector<int> copy_erase_list;
	std::vector<std::shared_ptr<class Character>> copy_Insert_list;
	std::vector<pair<PS, std::shared_ptr<flatbuffers::FlatBufferBuilder>>>  copy_send_all;
	{
		std::lock_guard<std::mutex> lock(mt);
		copy_erase_list.assign(req_erase_list.begin(), req_erase_list.end());
		copy_Insert_list.assign(req_Insert_list.begin(), req_Insert_list.end());
		copy_send_all.assign(req_send_all.begin(), req_send_all.end());
		req_erase_list.clear();
		req_Insert_list.clear();
		req_send_all.clear();
	}

	for (auto erase : copy_erase_list)
	{
		EraseCharacter(erase);
	}
	copy_erase_list.clear();

	for (auto insert : copy_Insert_list)
	{
		InsertCharacter(insert);
	}
	copy_Insert_list.clear();

	for (auto packet : copy_send_all)
	{
		SendAllCharacter(packet.first, packet.second);
	}
	copy_send_all.clear();
}

void CharacterManager::Tick()
{
	PrevTick();
	if (bMovable)
	{
		for (auto character : characters)
		{
			auto ch = character.second.lock();
			Moving(ch);
		}
	}	
}

void CharacterManager::EndPlay()
{
	for (auto character : characters)
	{

	}
}

CharacterManager::CharacterManager()
{
	bMovable = false;
}

CharacterManager::~CharacterManager()
{

}

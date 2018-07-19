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

void CharacterManager::Async_Function(std::shared_ptr<Func> func)
{
	async(std::launch::async, [this, func]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_func_list.push_back(func);
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
		character->cm = static_pointer_cast<CharacterManager>(shared_from_this());
		characters.insert(std::make_pair(character->code, character));
		return true;
	}
	return false;
}

void CharacterManager::SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
{	
	auto sb = std::make_shared<SendBuffer>(symbol, fbb);
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

int CharacterManager::Make_FBB_All_CharacterInfo(vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>>& fbbArray,int slice)
{
	vector<std::shared_ptr<vector<flatbuffers::Offset<FB::Character>>>> characterVecArray;

	std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
	std::shared_ptr<vector<flatbuffers::Offset<FB::Character>>> characterVec = std::make_shared<vector<flatbuffers::Offset<FB::Character>>>();

	int count = 0;
	int total = 0;

	for (auto character : characters)
	{
		auto ch = character.second.lock();		
		if (ch)
		{			
			auto nick = fbb->CreateString(ch->GetName());
			auto charB = FB::CharacterBuilder(*fbb);
			charB.add_code(ch->GetCode());
			charB.add_type(ch->GetTypeCode());
			charB.add_hp(ch->GetMaxHP());
			charB.add_level(ch->GetLevel());
			charB.add_nick(nick);
			charB.add_power(ch->GetPower());
			charB.add_speed(ch->GetSpeed());
			charB.add_position(&ch->GetPosition().ToFBVector3());
			characterVec->push_back(charB.Finish());
			count++;

			if (count == slice)
			{
				count = 0;
				fbbArray.push_back(fbb);
				characterVecArray.push_back(characterVec);
				auto vecoffset = fbb->CreateVector(*characterVec);
				fbb->Finish(FB::CreateCharacterVec(*fbb, vecoffset));
				fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
				characterVec = std::make_shared<vector<flatbuffers::Offset<FB::Character>>>();
			}
			total++;

			if (count != 0)
			{
				fbbArray.push_back(fbb);
				characterVecArray.push_back(characterVec);
				auto vecoffset = fbb->CreateVector(*characterVec);
				fbb->Finish(FB::CreateCharacterVec(*fbb, vecoffset));
			}
		}
	}
	return total;
}

bool CharacterManager::EraseCharacter(int key)
{
	auto result = characters.find(key);
	if (result != characters.end())
	{
		auto character = result->second.lock();
		if (character)
		{
			character->cm.reset();
		}
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
	std::vector<std::shared_ptr<Func>>  copy_func_list;
	{
		std::lock_guard<std::mutex> lock(mt);
		if (req_erase_list.empty()==false)
		{
			copy_erase_list.assign(req_erase_list.begin(), req_erase_list.end());
			req_erase_list.clear();
		}
		
		if (req_send_all.empty() == false)
		{
			copy_send_all.assign(req_send_all.begin(), req_send_all.end());
			req_send_all.clear();
		}
		
		if (req_Insert_list.empty() == false)
		{
			copy_Insert_list.assign(req_Insert_list.begin(), req_Insert_list.end());
			req_Insert_list.clear();

		}
		
		if (req_func_list.empty() == false)
		{
			copy_func_list.assign(req_func_list.begin(), req_func_list.end());
			req_func_list.clear();
		}		
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

	for (auto func : copy_func_list)
	{
		func->func();
	}
	copy_func_list.clear();

}

void CharacterManager::Tick()
{
	PrevTick();
	if (bMovable)
	{
		saveTime += Time::Delta();
		for (auto character : characters)
		{
			auto ch = character.second.lock();
			ch->Moving(Time::Delta());
		}

		/*if (saveTime > 0.4)
		{
			saveTime = 0;
			auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
			vector<flatbuffers::Offset<FB::Move>> moveVec;
			for (auto character : characters)
			{
				auto ch = character.second.lock();
				ch->GetMoveInfo(fbb, moveVec);
			}

			if (moveVec.size() > 0)
			{
				auto vec = fbb->CreateVector(moveVec);
				auto movebb = FB::CreateMoveVec(*fbb, vec);
				fbb->Finish(movebb);
				SendAllCharacter(PS::MOVING_VECTOR, fbb);
			}			
		}*/
		if (saveTime > 0.4)
		{
			saveTime = 0;
			vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>> fbbArray;
			vector<std::shared_ptr<vector<flatbuffers::Offset<FB::Move>>>> moveVecArray;
			std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
			std::shared_ptr<vector<flatbuffers::Offset<FB::Move>>> moveVec = std::make_shared<vector<flatbuffers::Offset<FB::Move>>>();
			int count = 0;
			int total = 0;

			for (auto character : characters)
			{
				auto mon = character.second.lock();
				mon->GetMoveInfo(fbb, *moveVec);
				count++;
				if (count == 10)
				{
					fbbArray.push_back(fbb);
					moveVecArray.push_back(moveVec);
					fbb = std::make_shared<flatbuffers::FlatBufferBuilder>(BUFSIZE);
					moveVec = std::make_shared<vector<flatbuffers::Offset<FB::Move>>>();
					count = 0;
				}
				total++;
			}

			if (count != 0)
			{
				fbbArray.push_back(fbb);
				moveVecArray.push_back(moveVec);
			}


			if (total > 0)
			{				
				for (int i = 0; i < fbbArray.size(); i++)
				{
					fbb = fbbArray[i];
					moveVec = moveVecArray[i];
					auto vec = fbb->CreateVector(*moveVec);
					auto movebb = FB::CreateMoveVec(*fbb, vec);
					fbb->Finish(movebb);
					SendAllCharacter(PS::MOVING_VECTOR, fbb);
				}
			}
		}
	}	
}

void CharacterManager::EndPlay()
{
	for (auto character : characters)
	{

	}
}

std::pair<int, float> CharacterManager::SearchNearPlayer(Vector3 vec)
{
	float min = MAX_flt;
	int code = -1;
	for (auto character : characters)
	{
		auto ch = character.second.lock();
		if (!ch) continue;
		float distance = Vector3::Distance(ch->GetPosition(), vec);
		if (distance < min)
		{
			min = distance;
			code = ch->GetCode();
		}
	}
	return make_pair(code, min);
}

std::shared_ptr<class Character> CharacterManager::GetCharacter(int key)
{
	auto result = characters.find(key);
	if (result != characters.end())
		return result->second.lock();
	return nullptr;
}

CharacterManager::CharacterManager()
{
	bMovable = false;
	saveTime = 0;
}

CharacterManager::~CharacterManager()
{

}


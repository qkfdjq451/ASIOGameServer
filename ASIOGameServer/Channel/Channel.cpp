#include "../Global.h"
#include "Channel.h"
#include "../Character/CharacterManager.h"
#include "../Character/Character.h"
#include "../User/User.h"
#include "../Asio/Session/Session.h"

Channel::Channel()
	:number(0)
{

}

void Channel::Async_InsertCharacter(std::shared_ptr<class Character> character)
{
	async(std::launch::async, [this, character]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_Insert_list.push_back(character);
	});
}

void Channel::Async_EraseCharacter(int key)
{
	async(std::launch::async, [this, key]
	{
		std::lock_guard<std::mutex> lock(mt);
		req_erase_list.push_back(key);
	});
}


void Channel::BeginPlay()
{	
	auto cm=Component::CreateComponent<CharacterManager>();
	cm->SetMovable(true);
	Attach(cm);
}

void Channel::PrevTick()
{
	std::vector<int> copy_erase_list;
	std::vector<std::shared_ptr<class Character>> copy_Insert_list;

	{
		std::lock_guard<std::mutex> lock(mt);
		copy_erase_list.assign(req_erase_list.begin(), req_erase_list.end());
		copy_Insert_list.assign(req_Insert_list.begin(), req_Insert_list.end());
		req_erase_list.clear();
		req_Insert_list.clear();
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
}

void Channel::Tick()
{
	PrevTick();

}

void Channel::EndPlay()
{

}

bool Channel::InsertCharacter(std::shared_ptr<class Character> character)
{
	auto cm = Component::GetComponent<CharacterManager>();
	bool result = cm->InsertCharacter(character);
	if(result)
	{
		auto fbb2 = cm->Make_FBB_All_CharacterInfo();
		auto user = character->GetUserPointer().lock();
		auto session = user->GetSesstion().lock();
		session->PushSend(PS::ENTER_NEW_CHARACTER_VECTOR, fbb2);

		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
		auto nick = fbb->CreateString(character->GetName());
		auto charB = FB::CharacterBuilder(*fbb);
		charB.add_code(character->GetCode());
		charB.add_hp(character->GetHP());
		charB.add_level(character->GetLevel());
		charB.add_nick(nick);
		charB.add_power(character->GetPower());
		charB.add_speed(character->GetSpeed());		
		auto charoffset=charB.Finish();
		fbb->Finish(charoffset);
		cm->SendAllCharacter(PS::ENTER_NEW_CHARACTER, move(fbb));

		return true;
	}
	return false;
}

bool Channel::EraseCharacter(int key)
{
	auto cm = Component::GetComponent<CharacterManager>();
	bool result = cm->EraseCharacter(key);
	if (result)
	{
		auto fbb = std::make_shared<flatbuffers::FlatBufferBuilder>();
		auto charB = FB::CharacterBuilder(*fbb);
		charB.add_code(key);
		auto charoffset = charB.Finish();
		fbb->Finish(charoffset);
		cm->SendAllCharacter(PS::LEAVE_CHARACTER, move(fbb));
		return true;
	}	
	return false;
}


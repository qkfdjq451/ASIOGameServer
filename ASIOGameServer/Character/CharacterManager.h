#pragma once

#include "../Global.h"

class CharacterManager : public Component
{
public:
	friend class Channel;

	//비동기 함수 (외부 쓰레드에서 호출시 반드시 위에 함수로 호출)
	void Async_InsertCharacter(std::shared_ptr<class Character> character);
	void Async_EraseCharacter(int key);
	void Async_SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	void Async_Function(std::shared_ptr<Func> func);

	int GetCharacterCount();	
private:

	bool InsertCharacter(std::shared_ptr<class Character> character);
	bool EraseCharacter(int key);	
	void SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	std::shared_ptr<flatbuffers::FlatBufferBuilder> Make_FBB_All_CharacterInfo();
	
private:
	bool Moving(std::shared_ptr<class Character> character);
public:
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;

	

	void SetMovable(bool _b) { bMovable = _b; }
private:
	std::unordered_map<int, std::weak_ptr<class Character>> characters;	
	std::vector<std::shared_ptr<class Character>> req_Insert_list;
	std::vector<int> req_erase_list;
	std::vector<pair<PS, std::shared_ptr<flatbuffers::FlatBufferBuilder>>> req_send_all;
	std::vector<std::shared_ptr<Func>> req_func_list;


	bool bMovable;
	float saveTime; 

	//생성자
public:
	CharacterManager();
	~CharacterManager();
};
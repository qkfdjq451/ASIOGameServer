#pragma once

#include "../Global.h"

class CharacterManager : public Component
{
public:
	friend class Channel;

	//�񵿱� �Լ� (�ܺ� �����忡�� ȣ��� �ݵ�� ���� �Լ��� ȣ��)
	void Async_InsertCharacter(std::shared_ptr<class Character> character);
	void Async_EraseCharacter(int key);
	void Async_SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	void Async_Function(std::shared_ptr<Func> func);

	int GetCharacterCount();	

	int Make_FBB_All_CharacterInfo(vector<std::shared_ptr<flatbuffers::FlatBufferBuilder>>&fbbArray, int slice=10);
private:

	bool InsertCharacter(std::shared_ptr<class Character> character);
	bool EraseCharacter(int key);	
	void SendAllCharacter(const PS & symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb);
	
private:
	bool Moving(std::shared_ptr<class Character> character);
public:
	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;
		

	void SetMovable(bool _b) { bMovable = _b; }

	std::pair<int, float> SearchNearPlayer(Vector3 vec);
	std::shared_ptr<class Character> GetCharacter(int key);
private:
	std::unordered_map<int, std::weak_ptr<class Character>> characters;	

	std::vector<std::shared_ptr<class Character>> req_Insert_list;
	std::vector<int> req_erase_list;
	std::vector<pair<PS, std::shared_ptr<flatbuffers::FlatBufferBuilder>>> req_send_all;
	std::vector<std::shared_ptr<Func>> req_func_list;


	bool bMovable;
	float saveTime; 

	//������
public:
	CharacterManager();
	~CharacterManager();
};
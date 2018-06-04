#pragma once

class Channel : public Component
{

public:
	Channel();
	//�񵿱� �Լ� (�ܺ� �����忡�� ȣ��� �ݵ�� ���� �Լ��� ȣ��)
	void Async_InsertCharacter(std::shared_ptr<class Character> character);
	void Async_EraseCharacter(int key);


	enum { Max_Character_Count = 50 };

	int GetNumber() { return number; }
	void SetNumber(int _number) { number = _number; }


	int GetMapCode() { return mapCode; }
	void SetMapCode(int _mapCode) { mapCode = _mapCode; }

	void BeginPlay() override;
	void PrevTick();
	void Tick() override;
	void EndPlay() override;

private:
	bool InsertCharacter(std::shared_ptr<class Character> character);
	bool EraseCharacter(int key);
private:
	int number;
	int mapCode;
		
	std::vector<std::shared_ptr<class Character>> req_Insert_list;
	std::vector<int> req_erase_list;
};
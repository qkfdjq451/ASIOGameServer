#pragma once

class Channel : public Component
{

public:
	Channel(int mapcode, int channelNumber);
	//비동기 함수 (외부 쓰레드에서 호출시 반드시 위에 함수로 호출)
	void Async_InsertCharacter(std::shared_ptr<class Character> character);
	void Async_EraseCharacter(int key);

	int GetNumber() { return number; }
	int GetMapCode() { return mapCode; }

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

	//삭제처리시 사용할 함수와 변수
public:
	float GetTime() { return time; }
	void AddTime(float addTime) { time += addTime; }
	void ResetTime() { time = 0; }
private:
	float time = 0;
};
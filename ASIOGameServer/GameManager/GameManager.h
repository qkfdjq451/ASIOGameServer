#pragma once


class GameManager :public Component
{
public:

		
//Override �Լ�
protected:
	void BeginPlay() override;
	void Tick() override;
	void EndPlay() override;

private:


	
public:
	GameManager();
	~GameManager();

};
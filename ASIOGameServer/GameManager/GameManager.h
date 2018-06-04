#pragma once


class GameManager :public Component
{
public:

		
//Override ÇÔ¼ö
protected:
	void BeginPlay() override;
	void Tick() override;
	void EndPlay() override;

private:


	
public:
	GameManager();
	~GameManager();

};
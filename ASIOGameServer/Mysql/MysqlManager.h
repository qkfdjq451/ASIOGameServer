#pragma once


class MySQLManager
{
public:
	static string Select_UserCharacter(int char_code);
	//NonBlocking
	// Async_Query 설명
	// 비동기로 호출 후 결과물이 있을 시 결과를 비동기로 호출한다
	// MysqlPool::QVecto = Query 결과 테이블
	// bool  = 성공 여부 
	void Async_Query(const char* sql, std::function<void(QVector&, bool)> func);

	//Blocking
	QVector Query(const char* sql);
	
private:
	std::unique_ptr<class MysqlPool> mysql_pool;


private:
	MySQLManager();
	~MySQLManager();
	static MySQLManager* inst;
public:
	static bool Create();
	static bool	Delete();
	static MySQLManager* Get() { return inst; }

};
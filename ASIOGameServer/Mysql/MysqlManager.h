#pragma once


class MySQLManager
{
public:
	static string Select_UserCharacter(int char_code);
	//NonBlocking
	// Async_Query ����
	// �񵿱�� ȣ�� �� ������� ���� �� ����� �񵿱�� ȣ���Ѵ�
	// MysqlPool::QVecto = Query ��� ���̺�
	// bool  = ���� ���� 
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
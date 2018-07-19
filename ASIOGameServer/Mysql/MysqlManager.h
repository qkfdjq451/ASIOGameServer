#pragma once


class MySQLManager
{
public:
	//NonBlocking
	// Async_Query ����
	// �񵿱�� ȣ�� �� ������� ���� �� ����� �񵿱�� ȣ���Ѵ�
	// MysqlPool::QVecto = Query ��� ���̺�
	// bool  = ���� ���� 
	void Async_Query(const char* sql, std::function<void(QVector&, bool)> func);
	void Async_Query_None_Select(const char* sql, std::function<void(bool)> func);
	void Async_Query_One_Select(const char* sql, std::function<void(string,bool)> func);

	//Blocking
	QVector Query_Vector(const char* sql);
	bool Query(const char* sql);
	bool Query(const char* sql,string& result);
	
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
#include "../Global.h"
#include "MysqlPool.h"
#include "MysqlManager.h"



MySQLManager::MySQLManager()
{
	mysql_pool = std::make_unique<MysqlPool>();
	mysql_pool->setParameter("35.201.212.220", "leedongin", "1234", "Paragon",3306);
}

MySQLManager::~MySQLManager()
{

}


void MySQLManager::Async_Query(const char * sql, std::function<void(QVector&, bool)> func)
{
	std::async(std::launch::async, [this, sql, func]
	{
		QVector vec;
		auto result = mysql_pool->executeSql_Vector(sql, std::ref(vec));
		func(std::ref(vec), result);
	});
}

void MySQLManager::Async_Query_None_Select(const char * sql, std::function<void(bool)> func)
{
	std::async(std::launch::async, [this, sql, func]
	{
		bool result = mysql_pool->executeSql(sql);
		func(result);
	});
}

void MySQLManager::Async_Query_One_Select(const char * sql, std::function<void(string, bool)> func)
{
	std::async(std::launch::async, [this, sql, func]
	{
		string str;
		bool result = mysql_pool->executeSql(sql, str);
		func(str,result);
	});
}


QVector MySQLManager::Query_Vector(const char * sql)
{
	QVector vec;
	auto result = mysql_pool->executeSql_Vector(sql, std::ref(vec));
	return vec;
}

bool MySQLManager::Query(const char * sql)
{
	return mysql_pool->executeSql(sql);
}

bool MySQLManager::Query(const char * sql, string & result)
{
	return mysql_pool->executeSql(sql, result);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
////SingleTon
/////////////////////////////////////////////////////////////////////////////////////////////////////////


MySQLManager* MySQLManager::inst = nullptr;

bool MySQLManager::Create()
{
	if (inst == nullptr)
	{
		inst = new MySQLManager();
		return true;
	}
	return false;
}

bool MySQLManager::Delete()
{
	if (inst != nullptr)
	{
		delete inst;
		inst = nullptr;
		return true;
	}
	return false;
}


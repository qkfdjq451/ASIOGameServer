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

const char * MySQLManager::Select_UserCharacter(int char_code)
{
	char sql[256];
	sprintf_s(sql,"select * from UserCharacter natural join CharacterPosion where UserCharacter.CharacterCode = %d;", char_code);
	return sql;
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

QVector MySQLManager::Query(const char * sql)
{
	QVector vec;
	auto result = mysql_pool->executeSql_Vector(sql, std::ref(vec));
	return std::move(vec);
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


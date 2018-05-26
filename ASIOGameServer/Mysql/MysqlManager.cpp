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

void MySQLManager::Query(const char * sql)
{
	mysql_pool->executeSql_Vector(sql);
}

void MySQLManager::Query_Result_Promise_Map(const char * sql, std::promise<QMap>& result)
{
	auto retval = mysql_pool->executeSql_Map(sql);
	result.set_value(move(retval));
}
void MySQLManager::Query_Result_Promise_Vector(const char * sql, std::promise<QVector>& result)
{
	auto retval = mysql_pool->executeSql_Vector(sql);
	result.set_value(move(retval));
}

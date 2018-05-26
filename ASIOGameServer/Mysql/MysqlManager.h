#pragma once

class MySQLManager :public Component, public std::enable_shared_from_this<MySQLManager>
{
public:
	
	MySQLManager();
	~MySQLManager();
	typedef std::map<const std::string, std::vector<const char*>> QMap;
	typedef std::vector<std::vector<std::string>> QVector;

	void Query(const char* sql);
	void Query_Result_Promise_Map(const char* sql, std::promise<QMap>& result);
	void Query_Result_Promise_Vector(const char* sql, std::promise<QVector>& result);
	
private:
	std::unique_ptr<class MysqlPool> mysql_pool;
};
#pragma once

#pragma comment(lib, "libmysql.lib")

#include <mysql.h>


class MysqlPool 
{
public:
	~MysqlPool();
	std::map<const std::string, std::vector<const char* > > executeSql(const char* sql);
	static MysqlPool* getMysqlPoolObject();
	void setParameter(const char*   _mysqlhost,
		const char*   _mysqluser,
		const char*   _mysqlpwd,
		const char*   _databasename,
		unsigned int  _port = 0,
		const char*   _socket = NULL,
		unsigned long _client_flag = 0,
		unsigned int  MAX_CONNECT = 50);
private:
	MysqlPool();
	MYSQL* createOneConnect();
	MYSQL* getOneConnect();
	void close(MYSQL* conn);
	bool isEmpty();
	MYSQL* poolFront();
	unsigned int poolSize();
	void poolPop();
private:
	std::queue<MYSQL*> mysqlpool;
	const char*   _mysqlhost;
	const char*   _mysqluser;
	const char*   _mysqlpwd;
	const char*   _databasename;
	unsigned int  _port;
	const char*   _socket;
	unsigned long _client_flag;
	unsigned int  MAX_CONNECT;
	unsigned int  connect_count;
	static std::mutex objectlock;
	static std::mutex poollock;
};

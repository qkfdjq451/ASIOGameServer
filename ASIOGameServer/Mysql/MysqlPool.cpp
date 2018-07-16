#include "../Global.h"
#include "MysqlManager.h"
#include "MysqlPool.h"


std::mutex MysqlPool::objectlock;
std::mutex MysqlPool::poollock;

MysqlPool::MysqlPool() {}

void MysqlPool::setParameter(
	const char*   mysqlhost,
	const char*   mysqluser,
	const char*   mysqlpwd,
	const char*   databasename,
	unsigned int  port,
	const char*   socket,
	unsigned long client_flag,
	unsigned int  max_connect) 
{
	_mysqlhost = mysqlhost;
	_mysqluser = mysqluser;
	_mysqlpwd = mysqlpwd;
	_databasename = databasename;
	_port = port;
	_socket = socket;
	_client_flag = client_flag;
	MAX_CONNECT = max_connect;
}

MYSQL* MysqlPool::createOneConnect() {
	MYSQL* conn = nullptr;
	conn = mysql_init(conn);
	if (conn != nullptr) {
		mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8");
		mysql_options(conn, MYSQL_INIT_COMMAND, "SET NAMES utf8");
		if (mysql_real_connect(
			conn,
			_mysqlhost,
			_mysqluser,
			_mysqlpwd,
			_databasename,
			_port,
			_socket,
			_client_flag)) {
			connect_count++;
			//c++ 기본 케릭터 set은 euckr이니 euckr로  설정
			//mysql_query(conn, "set session character_set_connection=euckr;");
			//mysql_query(conn, "set session character_set_results=euckr;");
			//mysql_query(conn, "set session character_set_client=euckr;");
			return conn;
		}
		else {
			std::cout << mysql_error(conn) << std::endl;
			return nullptr;
		}
	}
	else {
		std::cerr << "init failed" << std::endl;
		return nullptr;
	}
}

bool MysqlPool::isEmpty() {
	return mysqlpool.empty();
}

MYSQL* MysqlPool::poolFront() {
	return mysqlpool.front();
}

unsigned int MysqlPool::poolSize() {
	return mysqlpool.size();
}

void MysqlPool::poolPop() {
	mysqlpool.pop();
}

MYSQL* MysqlPool::getOneConnect() 
{
	poollock.lock();
	MYSQL *conn = nullptr;
	if (!isEmpty()) 
	{
		while (!isEmpty() && mysql_ping(poolFront())) 
		{
			mysql_close(poolFront());
			poolPop();
			connect_count--;
		}

		if (!isEmpty()) 
		{
			conn = poolFront();
			poolPop();
		}
		else 
		{
			if (connect_count < MAX_CONNECT)
				conn = createOneConnect();
			else
				std::cerr << "the number of mysql connections is too much!" << std::endl;
		}
	}
	else 
	{
		if (connect_count < MAX_CONNECT)
			conn = createOneConnect();
		else
			std::cerr << "the number of mysql connections is too much!" << std::endl;
	}
	poollock.unlock();
	return conn;
}

void MysqlPool::close(MYSQL* conn) 
{
	if (conn != nullptr)
	{
		poollock.lock();
		mysqlpool.push(conn);
		poollock.unlock();
	}
}

QMap MysqlPool::executeSql_Map(const char* sql)
{
	MYSQL* conn = getOneConnect();
	QMap results;
	if (conn) 
	{
		if (mysql_query(conn, sql) == 0) 
		{
			MYSQL_RES *res = mysql_store_result(conn);
			if (res) 
			{				
				MYSQL_FIELD *field;
				while ((field = mysql_fetch_field(res))) 
				{
					results.insert(make_pair(field->name, std::vector<const char*>()));
				}
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(res))) 
				{
					unsigned int i = 0;
					for (auto it = results.begin(); it != results.end(); ++it) 
					{
						(it->second).push_back(row[i++]);
					}
				}
				mysql_free_result(res);
			}
			else 
			{
				if (mysql_field_count(conn) != 0)
					std::cerr << mysql_error(conn) << std::endl;
			}
		}
		else 
		{
			std::cerr << mysql_error(conn) << std::endl;
		}
		close(conn);
	}
	else 
	{
		std::cerr << mysql_error(conn) << std::endl;
	}
	return results;
}

QVector MysqlPool::executeSql_Vector(const char * sql)
{
	MYSQL* conn = getOneConnect();
	QVector result;
	if (conn)
	{
		if (mysql_query(conn, sql) == 0)
		{
			MYSQL_RES *res = mysql_store_result(conn);
			if (res)
			{
				MYSQL_FIELD *field;
				std::vector<std::pair<std::string, std::string>> base;
				while ((field = mysql_fetch_field(res)))
				{
					base.push_back(std::make_pair(std::string(field->name), std::string("df")));
				}

				MYSQL_ROW row;
				int fieldCount = mysql_num_fields(res);
				while ((row = mysql_fetch_row(res)))
				{
					std::vector<std::pair<std::string, std::string>> clone(base);
					for (int i = 0; i < fieldCount; i++)
					{
						clone[i].second = row[i];
					}
					result.push_back(move(clone));
				}
			}
			else
			{
				std::cerr << mysql_error(conn) << std::endl;
			}
		}
		close(conn);
	}
	return result;
}

bool MysqlPool::executeSql_Vector(const char * sql, QVector& result)
{
	MYSQL* conn = getOneConnect();
	if (conn)
	{
		if (mysql_query(conn, sql) == 0)
		{
			MYSQL_RES *res = mysql_store_result(conn);
			MYSQL_FIELD *field;
			std::vector<std::pair<std::string, std::string>> base;			
			if (res)
			{
				while ((field = mysql_fetch_field(res)))
				{
					base.push_back(std::make_pair(std::string(field->name), std::string("df")));
				}

				MYSQL_ROW row;
				int fieldCount = mysql_num_fields(res);
				while ((row = mysql_fetch_row(res)))
				{
					std::vector<std::pair<std::string, std::string>> clone(base);					
					for (int i = 0; i < fieldCount; i++)
					{
						clone[i].second = row[i];
					}
					result.push_back(move(clone));
				}
			}			
		}
		else
		{
			//쿼리 실패 로그 남기기
			std::cerr << mysql_error(conn) << std::endl;
			return false;
		}
	}
	else
	{
		//connect 가져오기 실패 로그 남기기
		std::cerr << mysql_error(conn) << std::endl;
		return false;
	}
	return true;
}

bool MysqlPool::executeSql(const char * sql)
{
	MYSQL* conn = getOneConnect();
	if (conn)
	{		
		if (mysql_query(conn, sql) == 0)
		{
			close(conn);
			return true;
		}		
	}	
	std::cerr << mysql_error(conn) << std::endl;
	close(conn);
	return false;	
}

bool MysqlPool::executeSql(const char * sql, string & result)
{
	MYSQL* conn = getOneConnect();
	if (conn)
	{
		if (mysql_query(conn, sql) == 0)
		{
			MYSQL_RES *res = mysql_store_result(conn);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				result = row[0];
			}
			close(conn);
			return true;
		}
	}
	std::cerr << mysql_error(conn) << std::endl;
	close(conn);
	return false;
}

MysqlPool::~MysqlPool() 
{
	while (poolSize() != 0) 
	{
		mysql_close(poolFront());
		poolPop();
		connect_count--;
	}
}
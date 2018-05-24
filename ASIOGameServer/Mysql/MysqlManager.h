#pragma once

class MySQLManager : public Component
{
public:
	MySQLManager();
	~MySQLManager();

private:
	std::unique_ptr<class MysqlPool> mysql_pool;
};
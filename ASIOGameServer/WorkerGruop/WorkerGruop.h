#pragma once
class WorkerGruop
{
public:
	static WorkerGruop* Get_IOGroup() { return ioGroup.get(); }
	static WorkerGruop* Get_InGameGroup() { return ingameGroup.get(); }
	static WorkerGruop* Get_MySQL() { return mysqlGroup.get(); }
private:
	static std::unique_ptr<WorkerGruop> ioGroup;
	static std::unique_ptr<WorkerGruop> ingameGroup;
	static std::unique_ptr<WorkerGruop> mysqlGroup;
	
public:
	WorkerGruop(asio::io_service& io);
	~WorkerGruop();
	std::shared_ptr<class Component> GetRoot() { return root; }

	void PostFuction(std::shared_ptr<Func> func);
private:	
	asio::io_service::strand strand_;
	std::shared_ptr < class Component > root;

public:
	static void Create(asio::io_service& io);
};
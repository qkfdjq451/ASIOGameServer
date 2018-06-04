#pragma once
class WorkerGroup : public Component
{
public:
	static  std::shared_ptr<WorkerGroup> Get_IOGroup() { return ioGroup; }
	
private:
	static std::shared_ptr<WorkerGroup> ioGroup;
	
public:
	WorkerGroup(asio::io_service& io);
	~WorkerGroup();	

	void PostFuction(std::shared_ptr<Func> func);
private:	
	asio::io_service::strand strand_;
	

public:
	static void Create(asio::io_service& io);
};
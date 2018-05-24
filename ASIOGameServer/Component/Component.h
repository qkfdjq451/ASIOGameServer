#pragma once
class Component : public std::enable_shared_from_this<Component>
{
public:	
	Component() : parent(nullptr) {}
	~Component();

	void Run()
	{
		while (true)
		{
			Update();
		}
	}
	bool Attach(std::shared_ptr<Component> component);
	bool Detach(std::shared_ptr<Component> component);

	template<typename T>
	std::shared_ptr<T> GetComponent();

protected:
	virtual void Work(){};

	void Update()
	{
		Work();
		for (auto com : children)
		{
			com->Update();
		}
	}

private:
	std::shared_ptr<Component> parent;
	std::vector<std::shared_ptr<Component>> children;
};


template<typename T>
inline std::shared_ptr<T> Component::GetComponent()
{
	std::shared_ptr<T> retval = nullptr;
	for (auto com : children)
	{
		//해당 클래스가 맞는지 검사
		retval = std::dynamic_pointer_cast<T>(com);
		//자식도 검사
		if (retval == nullptr)
		{
			retval = com->GetComponent<T>();
		}
		//null이 아니면 바로 break;
		if (retval != nullptr)
		{
			break;
		}
	}
	return retval;
}

#pragma once

enum class ComponentState
{
	Create,
	Alive,
	Destroy
};

class Component : public std::enable_shared_from_this<Component>
{
public:
	
	Component();
	virtual ~Component();

	
	bool Attach(std::shared_ptr<Component> component);
	bool Detach(std::shared_ptr<Component> component);

	template<typename T>
	std::shared_ptr<T> GetComponent();

	template<typename T>
	void GetComponents(vector<std::shared_ptr<T>>& vec);

	template<typename T,
		class... _Types >
	static std::shared_ptr<T> CreateComponent(_Types&&... _Args);

	
	std::shared_ptr<Component> GetParentComponent() { return parent; }
	template<typename T>
	std::shared_ptr<T> GetParentComponent();

	class WorkerGroup*GetWorkerGroup() { return workerGroup; }
	void SetWorkerGroup(class WorkerGroup* _workerGroup) { workerGroup = _workerGroup; }
	
	//������û
	void Distroy();

	//

	

protected:
	virtual void BeginPlay() {};
	virtual void Tick(){};
	virtual void EndPlay() {};

public:
	virtual void Run() final;

	
private:	
	virtual void Update() final;
	virtual void Remove() final;
	
protected:
	class WorkerGroup* workerGroup;
	std::shared_ptr<Component> parent;
	std::list<std::shared_ptr<Component>> children;
	ComponentState state;


	//�������� �ҷ��� �� �ʿ��� ������
public:
	bool SetTag(string _tag);
	bool RemoveTag(string _tag);
	int GetID() { return id; }


	//����ȯ�ؼ� ����ؾ���
	static std::shared_ptr<Component> GetComponent_For_ID(int _id);
	static bool GetComponents_For_Tag(string _tag, std::vector<std::shared_ptr<Component>>& vec);


protected:
	std::mutex mt;

private:
	int id;
	set<std::string> tag;
	
	static int count;
	static std::map<int, std::shared_ptr<Component>> Components;
	static std::map<std::string, std::map<int, std::shared_ptr<Component>>> TagComponents;
};


template<typename T>
inline std::shared_ptr<T> Component::GetComponent()
{
	std::shared_ptr<T> retval = nullptr;
	for (auto com : children)
	{
		//�ش� Ŭ������ �´��� �˻�
		retval = std::dynamic_pointer_cast<T>(com);
		//�ڽĵ� �˻�
		if (retval == nullptr)
		{
			retval = com->GetComponent<T>();
		}
		//null�� �ƴϸ� �ٷ� break;
		if (retval != nullptr)
		{
			break;
		}
	}
	return retval;
}

template<typename T>
inline void Component::GetComponents(vector<std::shared_ptr<T>>& vec)
{	
	for (auto com : children)
	{
		//�ش� Ŭ������ �´��� �˻�
		auto retval = std::dynamic_pointer_cast<T>(com);		
		//����ȯ�� �Ϸ� ������ ���Ϳ� �ִ´�.

		if (retval != nullptr)		
			vec.push_back(retval);

		//�ڽĵ��� ������Ʈ�� �����Ѵ�.
		com->GetComponents<T>(vec);
	}
}

template<typename T,
	class... _Types >
inline std::shared_ptr<T> Component::CreateComponent(_Types&&... _Args)
{
	auto  make = std::make_shared<T>(_Args...);
	auto retval = std::dynamic_pointer_cast<Component>(make);
	retval->id = count;
	if (retval)
	{
		Components.insert(std::make_pair(count, retval));
		count++;
	}
	return make;
}

template<typename T>
inline std::shared_ptr<T> Component::GetParentComponent()
{
	return  std::dynamic_pointer_cast<T>(parent);	
}
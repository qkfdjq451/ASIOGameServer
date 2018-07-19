#include "../Global.h"
#include "Component.h"

int Component::count = 0;
std::map<int, std::shared_ptr<Component>> Component::Components;
std::map<std::string, std::map<int, std::shared_ptr<Component>>> Component::TagComponents;

Component::Component()
	: parent(nullptr), workerGroup(nullptr), state(ComponentState::Create)
{
	id = count++;
}

Component::~Component()
{	
}

void Component::Run()
{
	if (parent != nullptr)
		return;

	while (true)
	{
		Update();
	}
}

bool Component::Attach(std::shared_ptr<Component> component)
{
	if (component->parent==nullptr)
	{
		component->parent = shared_from_this();
		component->SetWorkerGroup(workerGroup);
		children.push_back(component);
		return true;
	}
	return false;
}

bool Component::Detach(std::shared_ptr<Component> component)
{
	for (auto iter = children.begin(); iter != children.end(); iter++)
	{
		if (component == *iter)
		{
			(*iter)->parent = nullptr;
			component->SetWorkerGroup(nullptr);
			children.erase(iter);
			return true;
		}
	}
	return false;
}

void Component::Distroy()
{
	state = ComponentState::Destroy;
	for (auto com : children)
	{
		com->Distroy();
	}
	int a = 10;
}

void Component::Update()
{	
	auto self = shared_from_this();

	if (state == ComponentState::Alive)
	{
		Tick();		
	}

	else if (state == ComponentState::Create)
	{
		BeginPlay();
		state = ComponentState::Alive;		
	}

	else if (state == ComponentState::Destroy)
	{
		EndPlay();
		Remove();
	}

	for (auto iter = children.begin();iter!=children.end();)
	{	
		auto com = (*iter);
		++iter;
		com->Update();
	}
}

void Component::Remove()
{
	if (parent)
	{
		//printf("테스트!!\n");
		parent->Detach(shared_from_this());
		Components.erase(id);
		//printf("테스트!23423!\n");
	}
	//for (auto com : children)
	//{
	//	com->Remove();
	//}	
	for (auto t : tag)
	{
		RemoveTag(t);
	}
}

bool Component::SetTag(string _tag)
{
	auto result = tag.insert(_tag);
	//태그 설정이 완료됐다면?
	if (result.second)
	{
		//기존에 같은 테그의 객체가 있는지 찾아본다.
		auto result = TagComponents.find(_tag);
		//만일 있다면?
		if (result != TagComponents.end())
		{
			result->second.insert(make_pair(id, shared_from_this()));
		}
		//없다면?
		else
		{
			std::map<int, std::shared_ptr<Component>> newMap;
			newMap.insert(make_pair(id, shared_from_this()));
			TagComponents.insert(make_pair(_tag, move(newMap)));
		}
		return true;
	}
	return false;
}

bool Component::RemoveTag(string _tag)
{
	auto result = tag.find(_tag);
	if (result != tag.end())
	{
		auto tagresult = TagComponents.find(_tag);
		if (tagresult != TagComponents.end())
		{
			tagresult->second.erase(id);
			return true;
		}
	}
	return false;
}

std::shared_ptr<Component> Component::GetComponent_For_ID(int _id)
{
	auto result = Components.find(_id);
	if (result != Components.end())
	{
		return result->second;
	}
	return nullptr;
}

bool Component::GetComponents_For_Tag(string _tag, std::vector<std::shared_ptr<Component>>& vec)
{	
	auto result = TagComponents.find(_tag);
	if (result != TagComponents.end())
	{
		for (auto com : result->second)
		{
			vec.push_back(com.second);
		}
		return true;
	}
	return false;
}



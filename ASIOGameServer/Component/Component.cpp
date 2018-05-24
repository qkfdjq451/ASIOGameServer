#include "../Global.h"
#include "Component.h"

Component::~Component()
{
	if (parent)
	{
		parent->Detach(shared_from_this());
	}
	//TODO : 자식들의 Component를 전부 지우는 작업
}

bool Component::Attach(std::shared_ptr<Component> component)
{
	if (component->parent==nullptr)
	{
		component->parent = shared_from_this();
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
			children.erase(iter);
			return true;
		}
	}
	return false;
}


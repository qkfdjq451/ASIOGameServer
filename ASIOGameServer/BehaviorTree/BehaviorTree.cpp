#include "../Global.h"
#include "BehaviorTree.h"

BehaviorTreeBuilder::BehaviorTreeBuilder()
{
	root = new BTRoot;
	parents.push(root);
}


BTNode * BehaviorTreeBuilder::Finish()
{
	if (parents.size() == 1)
		return root;
	return nullptr;
}

BehaviorTreeBuilder BehaviorTreeBuilder::end()
{
	parents.pop();
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::sequence()
{
	auto sq = new Sequence;
	if (parents.top()->type != NodeType::SELECTOR)
		parents.top()->Insert(sq);
	parents.push(sq);
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::sequence(int _key)
{
	auto sq = new Sequence;
	if (parents.top()->type == NodeType::SELECTOR)
		parents.top()->Insert(_key,sq);
	parents.push(sq);
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::selector(int* set_key)
{
	auto st = new Selector(set_key);
	if (parents.top()->type != NodeType::SELECTOR)
		parents.top()->Insert(st);
	parents.push(st);
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::selector(int key, int * set_key)
{
	auto st = new Selector(set_key);
	if (parents.top()->type == NodeType::SELECTOR)
		parents.top()->Insert(key,st);
	parents.push(st);
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::actor(std::function<bool(float)> func)
{
	auto act = new Actor(func);
	if(parents.top()->type!= NodeType::SELECTOR)
		parents.top()->Insert(act);
	return *this;
}

BehaviorTreeBuilder BehaviorTreeBuilder::actor(int key, std::function<bool(float)> func)
{
	auto act = new Actor(func);
	if (parents.top()->type == NodeType::SELECTOR)
		parents.top()->Insert(key, act);
	return *this;
}

Sequence::~Sequence()
{
	for (auto child : children)
	{
		delete child;
	}
}

Selector::~Selector()
{
	for (auto child : children)
	{
		delete child.second;
	}
}

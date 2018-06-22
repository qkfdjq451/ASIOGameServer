#pragma once
enum class NodeType : char
{
	BASE,
	ROOT,
	SEQUENCE,
	SELECTOR,
	ACTOR,
};
class BehaviorTreeBuilder
{
	BehaviorTreeBuilder();
public:
	static class BehaviorTreeBuilder Create() { return BehaviorTreeBuilder(); }

	class BTNode * Finish();
	BehaviorTreeBuilder end();
	BehaviorTreeBuilder sequence();
	BehaviorTreeBuilder sequence(int _key);
	BehaviorTreeBuilder selector(int* set_key);
	BehaviorTreeBuilder selector(int key ,int* set_key);
	BehaviorTreeBuilder actor(std::function<bool(float)> func);
	BehaviorTreeBuilder actor(int key ,std::function<bool(float)> func);
private:
	class BTNode * root;
	std::stack<class BTNode*> parents;
};

class BTNode
{
	friend class BehaviorTreeBuilder;
public:
	BTNode(NodeType type= NodeType::BASE) :type(type){}
	const NodeType type;
	virtual bool Run(float delta) { return true; };
	virtual void Insert(BTNode*btNode) {}
	virtual void Insert(int _key, BTNode*btNode) {}
};

class BTRoot : public BTNode
{
public:
	BTRoot(NodeType type = NodeType::ROOT) : BTNode(type){}
	friend class BehaviorTreeBuilder;
	virtual bool Run(float delta) { child->Run(delta); return true; }
	virtual void Insert(BTNode*btNode) { child = btNode; }
	~BTRoot() { delete child; }
private:
	BTNode * child;
};

class Sequence : public BTNode
{
public:
	Sequence(NodeType type = NodeType::SEQUENCE) : BTNode(type) {}
	~Sequence();
	virtual bool Run(float delta)
	{ 
		for (auto child : children)
		{
			if (!child->Run(delta))
				return false;
		}
		return true;
	}
	virtual void Insert(BTNode*btNode) { children.push_back(btNode); }
private:
	std::vector<BTNode*> children;
};

class Selector : public BTNode
{
public:
	Selector(int* key,NodeType type = NodeType::SELECTOR) : BTNode(type),key(key){}
	~Selector();
	virtual bool Run(float delta)
	{ 
		auto result = children.find(*key);
		if (result != children.end())
			return result->second->Run(delta);
		return false;
	};
	virtual void Insert(int _key,BTNode*btNode) 
	{ 
		auto result = children.insert(make_pair(_key,btNode)); 		
	}
private:
	int* key;
	std::map<int, BTNode*> children;
};

class Actor : public BTNode
{
public:
	Actor(NodeType type = NodeType::ACTOR) : BTNode(type) {}
	Actor(std::function<bool(float)> func) : func(func) {}
	virtual bool Run(float delta) { return func(delta); }
private:
	std::function<bool(float)> func;
};


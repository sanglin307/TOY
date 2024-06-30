#include "Private.h"

Layer::Layer(const std::string& name)
{
	_Name = name;
}

Layer::~Layer()
{
	for (auto n : _Nodes)
		delete n;
}

void Layer::AddNode(Node* node)
{
	_Nodes.push_back(node);
}

void Layer::Update(double delta)
{
	for (auto n : _Nodes)
	{
		n->Update(delta);
	}
}

Component* Layer::FindFirstComponent(ComponentType type)
{
	for (auto n : _Nodes)
	{
		Component* c = n->FindFirstComponent(type);
		if (c)
			return c;
	}

	return nullptr;
}
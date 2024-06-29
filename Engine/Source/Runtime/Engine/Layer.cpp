#include "Private.h"

Layer::Layer(const std::string& name)
{
	_Name = name;
}

void Layer::AddNode(Node* node)
{
	_Nodes.push_back(node);
}
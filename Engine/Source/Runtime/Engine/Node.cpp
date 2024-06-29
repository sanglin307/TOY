#include "Private.h"

Node::Node(const std::string& name)
{
	_Name = name;
}

void Node::SetTranslate(const glm::vec3& translate)
{
	_Translate = translate;
}

void Node::SetRotation(const glm::quat& rot)
{
	_Rotation = rot;
}

void Node::SetScale(const glm::vec3& scale)
{
	_Scale = scale;
}

void Node::AddChild(Node* node)
{
	check(std::find(_Children.begin(), _Children.end(), node) == _Children.end());
	_Children.push_back(node);
	node->SetParent(this);
}

void Node::SetParent(Node* node)
{
	_Parent = node;
}

void Node::Attach(Component* c)
{
	check(std::find(_Components.begin(), _Components.end(), c) == _Components.end());
	_Components.push_back(c);
	c->Attach(this);
}
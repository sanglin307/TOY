#include "Private.h"

Component::~Component()
{
}

void Component::Attach(Node* node)
{
	_Owner = node;
}

void PrimitiveComponent::RegisteToScene()
{
	GameWorld::Instance().GetScene()->AddPrimitive(this);
}
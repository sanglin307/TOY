#include "Private.h"

Component::~Component()
{
	UnRegiste();
}

void Component::Attach(Node* node)
{
	if (!_Owner)
	{
		_Owner = node;
		_Owner->Attach(this);
	}
}

void Component::Detach()
{
	if (_Owner)
	{
		Node* temp = _Owner;
		_Owner = nullptr;
		temp->Detach(this);
	}
}

void PrimitiveComponent::Registe()
{
	GameWorld::Instance().GetScene()->AddPrimitive(_Owner->GetTransform(),this);
}

void PrimitiveComponent::UnRegiste()
{
	GameWorld::Instance().GetScene()->RemovePrimitive(this);
}
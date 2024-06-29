#include "Private.h"

Component::~Component()
{
}

void Component::Attach(Node* node)
{
	_Owner = node;
}
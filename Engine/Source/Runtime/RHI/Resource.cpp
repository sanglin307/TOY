#include "Private.h"

RenderResource::RenderResource()
{
	RenderResourcePool::Instance().Registe(this);
}

RenderResource::~RenderResource()
{
	RenderResourcePool::Instance().UnRegiste(this);
}

RenderResourcePool& RenderResourcePool::Instance()
{
	static RenderResourcePool Inst;
	return Inst;
}

void RenderResourcePool::Registe(RenderResource* resource)
{
	if(!_DestroyProcess)
		_ResourceSet.insert(resource);
}

void RenderResourcePool::UnRegiste(RenderResource* resource)
{
	if(!_DestroyProcess)
		_ResourceSet.erase(resource);
}

void RenderResourcePool::Destroy()
{
	_DestroyProcess = true;
	for (auto iter = _ResourceSet.begin(); iter != _ResourceSet.end(); ++iter)
	{
		delete* iter;
	}
	_ResourceSet.clear();
	_DestroyProcess = false;
}
#include "Private.h"

RenderScene::RenderScene(GameWorld* world)
{
	_World = world;
}

RenderScene::~RenderScene()
{
}

void RenderScene::AddPrimitive(PrimitiveComponent* primitive)
{
	auto iter = std::find(_Primitives.begin(), _Primitives.end(), primitive);
	if (iter != _Primitives.end())
		return;

	_Primitives.push_back(primitive);
}

void RenderScene::RemovePrimitive(PrimitiveComponent* primitive)
{
	auto iter = std::find(_Primitives.begin(), _Primitives.end(), primitive);
	if (iter == _Primitives.end())
		return;

	_Primitives.erase(iter);
}
#include "Private.h"

LightComponent::LightComponent(const std::string& name, const Desc& desc)
{
	_Name = name;
	_Desc = desc;
}

void LightComponent::Registe()
{
	GameWorld::Instance().GetScene()->AddLight(_Owner->GetTransform(),this);
}

void LightComponent::UnRegiste()
{
	GameWorld::Instance().GetScene()->RemoveLight(this);
}
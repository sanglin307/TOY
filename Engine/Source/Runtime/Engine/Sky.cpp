#include "Private.h"

SkyComponent::SkyComponent(const std::string& name, const Desc& desc)
	:_Desc(desc), _Name(name)
{
}

void SkyComponent::SetDirectionalLight(LightComponent* light)
{
	check(light->GetDesc().Type == LightType::Directional);
	_DirectionalLight = light;
}

void SkyComponent::Registe()
{
	GameWorld::Instance().GetScene()->AddSky(this);
}

void SkyComponent::UnRegiste()
{
	GameWorld::Instance().GetScene()->RemoveSky(this);
}
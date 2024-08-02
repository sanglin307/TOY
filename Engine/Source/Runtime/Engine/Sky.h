#pragma once

class SkyComponent : public Component
{
public:
	struct Desc
	{
		float Rayleigh = 2.f;
		float Turbidity = 10.f;
		float MieCoefficient = 0.005f;
		float Luminance = 1.f;
		float MieDirectionalG = 0.8f;
	};

	virtual ComponentType GetComponentType() {
		return ComponentType::Sky;
	}

	virtual void Registe() override;
	virtual void UnRegiste() override;

	SkyComponent(const std::string& name, const Desc& desc);

	void SetDirectionalLight(LightComponent* light);
	LightComponent* GetDirectionalLight()
	{
		return _DirectionalLight;
	}

	Desc GetDesc() { return _Desc; }

private:
	Desc _Desc;
	std::string _Name;
	LightComponent* _DirectionalLight;
};
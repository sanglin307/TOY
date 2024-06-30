#pragma once

enum class CameraUpdateStrategy
{
	FirstPerson = 0,
	SphericalTarget,
};

enum class CameraType
{
	Perspective = 0,
	Orthographic
};

class Camera : public Component, public InputHandler
{
public:
	struct Desc
	{
		CameraType Type;
		union
		{
			float Width; // ortho
			float AspectRatio; // perspective
		};

		union
		{
			float Height; // ortho
			float YFov; // perspective
		};
		float ZFar;
		float ZNear;
	};

	Camera(const std::string& name, const Desc& desc);
	const std::string& GetName() const
	{
		return _Name;
	}

	virtual ComponentType GetComponentType() override
	{
		return ComponentType::Camera;
	}

	virtual bool OnKey(const KeyEvent& key) override;
	virtual bool OnMouse(const MouseEvent& mev) override;

	virtual void Update(double delta) override;

private:
	std::string  _Name;
	Desc _Desc;

};
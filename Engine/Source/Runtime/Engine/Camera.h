#pragma once

enum class CameraType
{
	Perspective = 0,
	Orthographic
};

class Camera : public Component
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
private:
	std::string  _Name;
	Desc _Desc;

};
#pragma once

enum class CameraType
{
	Perspective = 0,
	Orthographic
};

class CameraController;
class Camera : public Component
{
	friend class CameraController;
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

	void GetViewInfo(ViewInfo& info);
 
private:

	void InitCameraTransform();

	std::string  _Name;
	Desc _Desc;

	float3 _Position;
	quaternion _Rotation;

	float4x4 _ViewMatrix;
	float4x4 _ViewInverseMatrix;
	float4x4 _ProjectMatrix;
	float4x4 _ViewProjectMatrix;
};

class CameraController : public InputHandler
{
public:
	CameraController();
	~CameraController();
	void Attach(Camera* cam);

	virtual bool OnKey(const KeyEvent& key) override;
	virtual bool OnMouse(const MouseEvent& mev) override;

	virtual void Update(double delta) override;

protected:
	Camera* _Camera = nullptr;
	float3 _Velocity = {};

};
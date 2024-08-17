#pragma once

enum class CameraType
{
	Perspective = 0,
	Orthographic
};

class CameraController;
class CameraComponent : public Component
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

	CameraComponent(const std::string& name, const Desc& desc);
	const std::string& GetName() const
	{
		return _Name;
	}

	virtual ComponentType GetComponentType() override
	{
		return ComponentType::Camera;
	}

	void GetViewInfo(ViewInfo& info);

	void LookAt(const float3& eyePos, const float3& lookAt, const float3& up);
	void SetSpeed(float speed) { _Speed = speed; }
 
private:
	std::string  _Name;
	
	Desc _Info;

	float3 _EyePos;
	quaternion _Rot;
	float3 _ViewPos;
	//float3 _Up;
	//float3 _Right;

	float  _Distance;
	float  _Speed = 1.0f;

	float4x4 _PreViewMatrix;
	float4x4 _ViewMatrix;
	float4x4 _ViewInverseMatrix;
	float4x4 _ProjectMatrix;
	float4x4 _ViewProjectMatrix;


};

enum class CameraControlType
{
	Orbit,
	Free
};

class CameraController : public InputHandler
{
public:
	CameraController();
	~CameraController();
	void Attach(CameraComponent* cam);
	void Detach();

	virtual bool OnKey(const KeyEvent& key) override;
	virtual bool OnMouse(const MouseEvent& mev) override;

	virtual void Update(double delta) override;

protected:
	CameraComponent* _Camera = nullptr;
	CameraControlType _Type;
	float _MouseScrollDelta = 0.f;
};
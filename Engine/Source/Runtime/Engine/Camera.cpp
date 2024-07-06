#include "Private.h"

#if _DEBUG
#include <Windows.h>
#endif

using namespace hlslpp;

Camera::Camera(const std::string& name, const Desc& desc)
{
	_Name = name;
	_Desc = desc;
}

void Camera::InitCameraTransform()
{
	_Position = float3(0, 0, -10);
	_Rotation = quaternion(float3x3((float4x4::look_at(_Position, float3(0), float3(0, 1, 0)))));
}

void Camera::GetViewInfo(ViewInfo& info)
{
	info.View = _ViewMatrix;
	info.ViewInverse = _ViewInverseMatrix;
	info.Project = _ProjectMatrix;
	info.ProjectInverse = inverse(_ProjectMatrix);
	info.ViewProject = _ViewProjectMatrix;
	info.ViewProjectInverse = inverse(_ViewProjectMatrix);
	info.ViewLocation = _Position;
}

bool CameraController::OnKey(const KeyEvent& key)
{
	return true;
}

bool CameraController::OnMouse(const MouseEvent& mev)
{
	return true;
}

float3 SmoothStep(const float3& v1, const float3& v2, float t)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	return lerp(v1, v2, t);
}

void CameraController::Update(double delta)
{
	if (!_Camera)
		return;

	float3 movement = {};
	static int2 mousePrePos = InputManager::Instance().GetMousePostion();
	if (InputManager::Instance().IsKeyDown(KeyType::LeftMouseButton))
	{
		int2 mouseDelta = InputManager::Instance().GetMousePostion() - mousePrePos;
		mousePrePos = InputManager::Instance().GetMousePostion();
		//OutputDebugStringA(std::format("delta: {},{}\n", mouseDelta[0], mouseDelta[1]).c_str());
		quaternion pitch = quaternion::rotation_euler_zxy(float3(mouseDelta[1] * delta * 0.2f, 0.f,0.f));
		quaternion yaw = quaternion::rotation_euler_zxy(float3(0.f, mouseDelta[0] * delta * 0.2f, 0.f));
		_Camera->_Rotation = mul(mul(pitch, _Camera->_Rotation),yaw);
		 

		if (InputManager::Instance().IsKeyDown(KeyType::A))
			movement.x -= 1;
		if (InputManager::Instance().IsKeyDown(KeyType::D))
			movement.x += 1;
		if (InputManager::Instance().IsKeyDown(KeyType::S))
			movement.z -= 1;
		if (InputManager::Instance().IsKeyDown(KeyType::W))
			movement.z += 1;
		if (InputManager::Instance().IsKeyDown(KeyType::Q))
			movement.y -= 1;
		if (InputManager::Instance().IsKeyDown(KeyType::E))
			movement.y += 1;

		movement = mul(movement, _Camera->_Rotation); 
	}
	else
	{
		mousePrePos = InputManager::Instance().GetMousePostion();
	}

	_Velocity = SmoothStep(_Velocity, movement, 0.2f);
	_Camera->_Position += _Velocity * float(delta) * 4.0f;

	
	_Camera->_ViewInverseMatrix = mul(float4x4(_Camera->_Rotation), float4x4::translation(_Camera->_Position));
	_Camera->_ViewMatrix = inverse(_Camera->_ViewInverseMatrix);
	
	uint2 viewportSize = GameEngine::Instance().GetViewport()->GetSize();
	float aspectRatio = viewportSize[0] * 1.f / viewportSize[1];

	if (_Camera->_Desc.Type == CameraType::Perspective)
	{
		frustum persp = frustum::field_of_view_y(_Camera->_Desc.YFov, _Camera->_Desc.AspectRatio, _Camera->_Desc.ZNear, _Camera->_Desc.ZFar);
		_Camera->_ProjectMatrix = float4x4::perspective(projection(persp, zclip::zero, zdirection::reverse, _Camera->_Desc.ZFar <= 0 ? zplane::infinite: zplane::finite));
		
	}
	else if(_Camera->_Desc.Type == CameraType::Orthographic)
	{
		frustum ortho(_Camera->_Desc.Width, _Camera->_Desc.Height, _Camera->_Desc.ZNear, _Camera->_Desc.ZFar);
		_Camera->_ProjectMatrix = float4x4::orthographic(projection(ortho, zclip::zero));
	}
	else
	{
		check(0);
	}
	_Camera->_ViewProjectMatrix = mul(_Camera->_ViewMatrix, _Camera->_ProjectMatrix);
}



void CameraController::Attach(Camera* cam)
{
	_Camera = cam;
	_Camera->InitCameraTransform();
}

CameraController::CameraController()
{
	InputManager::Instance().AddHandler(this);
}

CameraController::~CameraController()
{
	InputManager::Instance().RemoveHandler(this);
}
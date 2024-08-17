#include "Private.h"

#if _DEBUG
#include <Windows.h>
#endif
 
using namespace hlslpp;

CameraComponent::CameraComponent(const std::string& name, const Desc& desc)
{
	_Name = name;
	_Info = desc;
}


void CameraComponent::LookAt(const float3& eyePos, const float3& lookAt, const float3& up)
{
	_EyePos = eyePos;
	_ViewPos = lookAt;
	_Distance = length(_ViewPos - _EyePos);
	_ViewMatrix = float4x4::look_at(_EyePos, lookAt, up);
	_ViewInverseMatrix = inverse(_ViewMatrix);
	_Rot = quaternion(float3x3(_ViewInverseMatrix));
}


void CameraComponent::GetViewInfo(ViewInfo& info)
{
	info.View = _ViewMatrix;
	info.ViewInverse = _ViewInverseMatrix;
	info.Project = _ProjectMatrix;
	info.ProjectInverse = inverse(_ProjectMatrix);
	info.ViewProject = _ViewProjectMatrix;
	info.ViewProjectInverse = inverse(_ViewProjectMatrix);
	info.ViewLocation = _EyePos;
}

bool CameraController::OnKey(const KeyEvent& key)
{
	return true;
}

bool CameraController::OnMouse(const MouseEvent& mev)
{
	if (mev.Key == KeyType::MouseScrollUp || mev.Key == KeyType::MouseScrollDown)
	{
		_MouseScrollDelta = mev.ScrollDelta;
	}
	else
	{
		_MouseScrollDelta = 0.f;
	}
 

	return true;
}
 
void CameraController::Update(double delta)
{
	if (!_Camera)
		return;

	static int2 mousePrePos = InputManager::Instance().GetMousePostion();
	if (InputManager::Instance().IsKeyDown(KeyType::LeftMouseButton))
	{
		int2 mouseDelta = InputManager::Instance().GetMousePostion() - mousePrePos;
		mousePrePos = InputManager::Instance().GetMousePostion();
		if (_Type == CameraControlType::Orbit)
		{
			if (mouseDelta[0] != 0 || mouseDelta[1] != 0 || _MouseScrollDelta != 0.f)
			{
				float yAngle = 0.f;
				float zAngle = 0.f;
	 
				Math::ConvertToSphericalCoord(_Camera->_EyePos, _Camera->_ViewPos, _Camera->_Distance, yAngle, zAngle);
 
			    zAngle -= float(delta * _Camera->_Speed * mouseDelta[0]);
			    yAngle += float(delta * _Camera->_Speed * mouseDelta[1]);

				_Camera->_Distance -= float(_MouseScrollDelta * delta * 8);

				if (_Camera->_Distance < 2)
					_Camera->_Distance = 2;
				if (_Camera->_Distance > 500)
					_Camera->_Distance = 500;

				float limit = 3.0f;
				limit = limit * Math::Deg2Rad();

				if (yAngle < limit)
					yAngle = limit;

				if (yAngle > Math::Pi() - limit)
					yAngle = Math::Pi() - limit;

				float3 finalPos;
				Math::ConvertFromSphericalCoord(_Camera->_Distance, yAngle, zAngle, _Camera->_ViewPos, finalPos);

				_Camera->LookAt(finalPos, _Camera->_ViewPos, float3(0, 1, 0));
			}
		}
		else if (_Type == CameraControlType::Free)
		{
			float3 t = {};
			if (InputManager::Instance().IsKeyDown(KeyType::A))
			{
				t.x = -_Camera->_Speed * 10 * delta;
			}

			if (InputManager::Instance().IsKeyDown(KeyType::D))
			{
				t.x = _Camera->_Speed * 10 * delta;
			}

			if (InputManager::Instance().IsKeyDown(KeyType::S))
			{
				t.z = -_Camera->_Speed * 10 * delta;
			}

			if (InputManager::Instance().IsKeyDown(KeyType::W))
			{
				t.z = _Camera->_Speed * 10 * delta;
			}

			if (InputManager::Instance().IsKeyDown(KeyType::Q))
			{
				t.y = _Camera->_Speed * 10 * delta;
			}

			if (InputManager::Instance().IsKeyDown(KeyType::E))
			{
				t.y = -_Camera->_Speed * 10 * delta;
			}

			quaternion rx = quaternion::identity();
			quaternion ry = quaternion::identity();
			if (mouseDelta[0] != 0)
			{
				float c = (float)(mouseDelta[0] * _Camera->_Speed * delta * 0.3);
				rx = quaternion::rotation_y(c);
			}

			if (mouseDelta[1] != 0)
			{
				float c = (float)(mouseDelta[1] * _Camera->_Speed * delta * 0.3);
				ry = quaternion::rotation_x(c);
			}				
			
			_Camera->_Rot = mul(rx, _Camera->_Rot);
			_Camera->_Rot = mul(_Camera->_Rot,ry);
			t = mul(t, _Camera->_Rot);
			_Camera->_EyePos += t;
			_Camera->_ViewInverseMatrix = mul(float4x4(_Camera->_Rot), float4x4::translation(_Camera->_EyePos));
			_Camera->_ViewMatrix = inverse(_Camera->_ViewInverseMatrix);
		}
	}
	else
	{
		mousePrePos = InputManager::Instance().GetMousePostion();
	}

	_MouseScrollDelta = 0.f;

	_Camera->_ViewInverseMatrix = inverse(_Camera->_ViewMatrix);

	if (_Camera->_Info.Type == CameraType::Perspective)
	{
		frustum persp = frustum::field_of_view_x(_Camera->_Info.YFov, _Camera->_Info.AspectRatio, _Camera->_Info.ZFar, _Camera->_Info.ZNear);
		_Camera->_ProjectMatrix = float4x4::perspective(projection(persp, zclip::zero, zdirection::reverse,  zplane::finite));		
	}
	else if(_Camera->_Info.Type == CameraType::Orthographic)
	{
		frustum ortho(_Camera->_Info.Width, _Camera->_Info.Height, _Camera->_Info.ZNear, _Camera->_Info.ZFar);
		_Camera->_ProjectMatrix = float4x4::orthographic(projection(ortho, zclip::zero));
	}
	else
	{
		check(0);
	}
	_Camera->_ViewProjectMatrix = mul(_Camera->_ViewMatrix, _Camera->_ProjectMatrix);
}



void CameraController::Attach(CameraComponent* cam)
{
	_Camera = cam;
}

void CameraController::Detach()
{
	_Camera = nullptr;
}

CameraController::CameraController()
{
	InputManager::Instance().AddHandler(this);
	_Type = CameraControlType::Free;
}

CameraController::~CameraController()
{
	InputManager::Instance().RemoveHandler(this);
}
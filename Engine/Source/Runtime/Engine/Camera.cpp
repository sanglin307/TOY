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
	_Direction = lookAt - eyePos;
	_Distance = length(_Direction);
	_Direction = normalize(_Direction);
	_Right = cross(up, _Direction);
	_Right = normalize(_Right);
	_Up = cross(_Direction, _Right);

	_ViewMatrix = float4x4::look_at(_EyePos, lookAt, _Up);
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
	 
				float3 target = _Camera->_EyePos + _Camera->_Direction * _Camera->_Distance;
				Math::ConvertToSphericalCoord(_Camera->_EyePos, target, _Camera->_Distance, yAngle, zAngle);
 
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
				Math::ConvertFromSphericalCoord(_Camera->_Distance, yAngle, zAngle, target, finalPos);

				_Camera->LookAt(finalPos, target, float3(0, 1, 0));
			}
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
		frustum persp = frustum::field_of_view_y(_Camera->_Info.YFov, _Camera->_Info.AspectRatio, _Camera->_Info.ZFar, _Camera->_Info.ZNear);
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
	_Type = CameraControlType::Orbit;
}

CameraController::~CameraController()
{
	InputManager::Instance().RemoveHandler(this);
}
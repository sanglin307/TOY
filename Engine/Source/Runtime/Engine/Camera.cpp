#include "Private.h"

Camera::Camera(const std::string& name, const Desc& desc)
{
	_Name = name;
	_Desc = desc;
}

bool Camera::OnKey(const KeyEvent& key)
{
	return true;
}

bool Camera::OnMouse(const MouseEvent& mev)
{
	return true;
}

void Camera::Update(double delta)
{

}
#include "Private.h"

//template<> const Vector3<float> Vector3<float>::Zero = Vector3f(0.f,0.f,0.f) ;
//template<> const Vector3<float> Vector3<float>::One = Vector3f(1.f,1.f,1.f);
//template<> const Vector3<float> Vector3<float>::UnitX = Vector3f(1.f,0.f,0.f);
//template<> const Vector3<float> Vector3<float>::UnitY = Vector3f(0.f,1.f,0.f);
//template<> const Vector3<float> Vector3<float>::UnitZ = Vector3f(0.f,0.f,1.f);
// 

const float3& Coordinate::Zero()
{
	static float3 temp(0.f, 0.f, 0.f);
	return temp;
}

const float3& Coordinate::One()
{
	static float3 temp(1.f, 1.f, 1.f);
	return temp;
}

const float3& Coordinate::UnitX()
{
	static float3 temp(1.f, 0.f, 0.f);
	return temp;
}

const float3& Coordinate::UnitY()
{
	static float3 temp(0.f, 1.f, 0.f);
	return temp;
}

const float3& Coordinate::UnitZ()
{
	static float3 temp(0.f, 0.f, 1.f);
	return temp;
}

//// left hand, up for Y axis.
const float3& Coordinate::Up()
{
	static float3 temp(0.f, 1.f, 0.f);
	return temp;
}

const float3& Coordinate::Down()
{
	static float3 temp(0.f, -1.f, 0.f);
	return temp;
}

const float3& Coordinate::Right()
{
	static float3 temp(1.f, 0.f, 0.f);
	return temp;
}

const float3& Coordinate::Left()
{
	static float3 temp(-1.f, 0.f, 0.f);
	return temp;
}

const float3& Coordinate::Forward()
{
	static float3 temp(0.f, 0.f, 1.f);
	return temp;
}

const float3& Coordinate::Backward()
{
	static float3 temp(0.f, 0.f, -1.f);
	return temp;
}
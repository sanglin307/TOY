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

void Math::ConvertToSphericalCoord(const float3& pos, const float3& origin, float& radius, float& yAngle, float& zAngle)
{
	float3 deltaVec = pos - origin;
	radius = length(deltaVec);
	yAngle = std::acosf(deltaVec.y / radius);
	zAngle = std::atan2(deltaVec.x, deltaVec.z);
}

void Math::ConvertFromSphericalCoord(float radius, float yAngle, float zAngle, const float3& origin, float3& pos)
{
	check(radius > 0);
	float temp = radius * std::sinf(yAngle);

	pos.z = temp * std::cosf(zAngle);
	pos.x = temp * std::sinf(zAngle);
	pos.y = radius * std::cosf(yAngle);

	pos += origin;
}

// From Eigen implement
float3 Math::EulerAngles(const float4x4& mat, int a0, int a1, int a2)
{
	/* Implemented from Graphics Gems IV */
	float3 res;

	const int odd = ((a0 + 1) % 3 == a1) ? 0 : 1;
	const int i = a0;
	const int j = (a0 + 1 + odd) % 3;
	const int k = (a0 + 2 - odd) % 3;

	if (a0 == a2)
	{
		res[0] = std::atan2(mat[i][j], mat[i][k]);
		if ((odd && res[0] < 0) || ((!odd) && res[0] > 0))
		{
			res[0] = (res[0] > 0) ? res[0] - Pi() : res[0] + Pi();
			float s2 = length(float2(mat[i][j], mat[i][k]));
			res[1] = -std::atan2(s2, mat[i][i]);
		}
		else
		{
			float s2 = length(float2(mat[i][j], mat[i][k]));
			res[1] = std::atan2(s2, mat[i][i]);
		}

		// With a=(0,1,0), we have i=0; j=1; k=2, and after computing the first two angles,
		// we can compute their respective rotation, and apply its inverse to M. Since the result must
		// be a rotation around x, we have:
		//
		//  c2  s1.s2 c1.s2                   1  0   0 
		//  0   c1    -s1       *    M    =   0  c3  s3
		//  -s2 s1.c2 c1.c2                   0 -s3  c3
		//
		//  Thus:  m11.c1 - m21.s1 = c3  &   m12.c1 - m22.s1 = s3

		float s1 = std::sin(res[0]);
		float c1 = std::cos(res[0]);
		res[2] = std::atan2(c1 * mat[k][j] - s1 * mat[k][k], c1 * mat[j][j] - s1 * mat[j][k]);
	}
	else
	{
		res[0] = std::atan2(mat[k][j], mat[k][k]);
		float c2 = length(float2(mat[i][i], mat[j][i]));
		if ((odd && res[0] < 0) || ((!odd) && res[0] > 0)) {
			res[0] = (res[0] > 0) ? res[0] - Pi() : res[0] + Pi();
			res[1] = std::atan2(-mat[k][i], -c2);
		}
		else
			res[1] = std::atan2(-mat[k][i], c2);
		float s1 = std::sin(res[0]);
		float c1 = std::cos(res[0]);
		res[2] = std::atan2(s1 * mat[i][k] - c1 * mat[i][j], c1 * mat[j][j] - s1 * mat[j][k]);
	}
	if (!odd)
		res = -res;

	return res;
}
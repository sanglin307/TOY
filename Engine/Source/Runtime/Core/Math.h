#pragma once

constexpr float Pi()
{
	return 3.14159265f;
}

struct Vector2
{
	Vector2(float v)
	{
		f[0] = f[1] = v;
	}

	Vector2(float x, float y)
	{
		f[0] = x;
		f[1] = y;
	}

	float& operator[](int N)
	{
		return f[N];
	}

	const float& operator[](int N) const
	{
		return f[N];
	}

	float f[2];
};

struct Vector3
{
	Vector3(float v)
	{
		f[0] = f[1] = f[2] = v;
	}

	Vector3(float x, float y, float z)
	{
		f[0] = x;
		f[1] = y;
		f[2] = z;
	}

	float& operator[](int N)
	{
		return f[N];
	}

	const float& operator[](int N) const
	{
		return f[N];
	}

	float f[3];
};

struct Vector4
{
	Vector4(float v)
	{
		f[0] = f[1] = f[2] = f[3] = v;
	}

	Vector4(float x, float y, float z,float w)
	{
		f[0] = x;
		f[1] = y;
		f[2] = z;
		f[3] = w;
	}

	float& operator[](int N)
	{
		return f[N];
	}

	const float& operator[](int N) const
	{
		return f[N];
	}

	float f[4];
};
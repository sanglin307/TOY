#pragma once

constexpr float Pi()
{
	return 3.14159265f;
}

template<typename T>
struct Vector2
{
	Vector2(T v)
	{
		f[0] = f[1] = v;
	}

	Vector2(T x, T y)
	{
		f[0] = x;
		f[1] = y;
	}

	T& operator[](int N)
	{
		return f[N];
	}

	const T& operator[](int N) const
	{
		return f[N];
	}

	T f[2];
};

using Vector2f = Vector2<float>;
using Vector2u = Vector2<u32>;
using Vector2i = Vector2<i32>;

template<typename T>
struct Vector3
{
	Vector3(T v)
	{
		f[0] = f[1] = f[2] = v;
	}

	Vector3(T x, T y, T z)
	{
		f[0] = x;
		f[1] = y;
		f[2] = z;
	}

	T& operator[](int N)
	{
		return f[N];
	}

	const T& operator[](int N) const
	{
		return f[N];
	}

	T f[3];
};

using Vector3f = Vector3<float>;
using Vector3u = Vector3<u32>;
using Vector3i = Vector3<i32>;

template<typename T>
struct Vector4
{
	Vector4(T v)
	{
		f[0] = f[1] = f[2] = f[3] = v;
	}

	Vector4(T x, T y, T z,T w)
	{
		f[0] = x;
		f[1] = y;
		f[2] = z;
		f[3] = w;
	}

	T& operator[](int N)
	{
		return f[N];
	}

	const T& operator[](int N) const
	{
		return f[N];
	}

	T f[4];
};

using Vector4f = Vector4<float>;
using Vector4u = Vector4<u32>;
using Vector4i = Vector4<i32>;


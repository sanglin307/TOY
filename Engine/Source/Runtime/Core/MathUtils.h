#pragma once

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

struct Coordinate
{
	CORE_API static const float3& Zero();
	CORE_API static const float3& One();
	CORE_API static const float3& UnitX();
	CORE_API static const float3& UnitY();
	CORE_API static const float3& UnitZ();

	CORE_API static const float3& Up();
	CORE_API static const float3& Down();
	CORE_API static const float3& Right();
	CORE_API static const float3& Left();
	CORE_API static const float3& Forward();
	CORE_API static const float3& Backward();
};

struct CORE_API Math
{
public:

	static constexpr float Pi()
	{
		return 3.14159265f;
	}

	static constexpr float PiDiv2()
	{
		return 1.570796327f;
	}

	static constexpr float Deg2Rad()
	{
		return Pi() / float(180.0);
	} 

	static constexpr float Rad2Deg()
	{
		return float(180.0) / Pi();
	}

	static inline unsigned int LogTwo(unsigned int n)
	{
		int i = 1;
		while (n / 2 > 1)
		{
			i++;
			n /= 2;
		}

		return i;
	}

	/** Returns the closest power-of-two number greater or equal to value.
		@note 0 and 1 are powers of two, so
			FirstPOW2From(0)==0 and FirstPOW2From(1)==1.
	*/

	static inline unsigned int FirstPOW2From(unsigned int n)
	{
		--n;
		n |= n >> 16;
		n |= n >> 8;
		n |= n >> 4;
		n |= n >> 2;
		n |= n >> 1;
		++n;
		return n;
	}

	/** Determines whether the number is power-of-two or not.
		@note 0 and 1 are tread as power of two.
	*/
	template<typename T>
	static inline bool IsPOW2(T n)
	{
		return (n & (n - 1)) == 0;
	}


	static inline bool IsNaN(float fx)
	{
		return _isnan(fx) != 0;
	}

	static inline bool IsFinite(float fx)
	{
		return _finite(fx) != 0;
	}

	static inline bool FloatEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon())
	{
		if (fabs(b - a) <= tolerance)
			return true;
		else
			return false;
	}

	static inline float UnitRandom()
	{
		return float(rand()) / (float)RAND_MAX;
	}


	static  void ConvertToSphericalCoord(const float3& pos, const float3& origin, float& radius, float& yAngle, float& zAngle);
	static  void ConvertFromSphericalCoord(float radius, float yAngle, float zAngle, const float3& origin, float3& pos);
	static float3 EulerAngles(const float4x4& mat, int a0, int a1, int a2);
};
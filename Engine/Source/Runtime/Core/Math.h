#pragma once

//refer to PBRT v4
template <template <typename> class Child, typename T>
class Tuple3 
{
public:
    Tuple3() = default;
    Tuple3(T x, T y, T z) : x(x), y(y), z(z) {}
 
    T operator[](i32 i) const 
    {
        check(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    T& operator[](i32 i) 
    {
        check(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    template <typename U>
    auto operator+(Child<U> c) const->Child<decltype(T{} + U{}) > 
    {
        return { x + c.x, y + c.y, z + c.z };
    }

    template <typename U>
    Child<T>& operator+=(Child<U> c) 
    {
        x += c.x;
        y += c.y;
        z += c.z;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    auto operator-(Child<U> c) const->Child<decltype(T{} - U{}) > 
    {
        return { x - c.x, y - c.y, z - c.z };
    }

    template <typename U>
    Child<T>& operator-=(Child<U> c) 
    {
        x -= c.x;
        y -= c.y;
        z -= c.z;
        return static_cast<Child<T> &>(*this);
    }

    bool operator==(Child<T> c) const { return x == c.x && y == c.y && z == c.z; }
    bool operator!=(Child<T> c) const { return x != c.x || y != c.y || z != c.z; }

    template <typename U>
    auto operator*(U s) const->Child<decltype(T{} *U{}) > 
    {
        return { s * x, s * y, s * z };
    }

    template <typename U>
    Child<T>& operator*=(U s) 
    {
        x *= s;
        y *= s;
        z *= s;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    auto operator/(U d) const->Child<decltype(T{} / U{}) > 
    {
        return { x / d, y / d, z / d };
    }

    template <typename U>
    Child<T>& operator/=(U d) 
    {
        check(d != 0);
        x /= d;
        y /= d;
        z /= d;
        return static_cast<Child<T> &>(*this);
    }
    
    Child<T> operator-() const { return { -x, -y, -z }; }

    std::string ToString() const { return std::format("{},{},{}", x, y, z); }
 
    T x{}, y{}, z{};
};

template <typename T>
class Vector3 : public Tuple3<Vector3, T> {
public:
    using Tuple3<Vector3, T>::x;
    using Tuple3<Vector3, T>::y;
    using Tuple3<Vector3, T>::z;

    Vector3() = default;
    Vector3(T x, T y, T z) : Tuple3<Vector3, T>(x, y, z) {}

    template <typename U>
    explicit Vector3(Vector3<U> v)
          : Tuple3<Vector3, T>(T(v.x), T(v.y), T(v.z)) 
    {}

};

using Vector3f = Vector3<f32>;
using Vector3d = Vector3<f64>;
using Vector3i = Vector3<i32>;
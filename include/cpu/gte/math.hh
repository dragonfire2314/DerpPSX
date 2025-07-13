#ifndef MATH_hpp 
#define MATH_hpp

#include <global.hh>

template <typename T>
struct Vector3
{
    T x, y, z;

    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vector3() : x(0), y(0), z(0) {}
};

template <typename T>
struct Vector4
{
    T x, y, z, w;

    Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4() : x(0), y(0), z(0), w(0) {}
};

struct Matrix3x3
{
    sh M11, M12, M13;
    sh M21, M22, M23;
    sh M31, M32, M33;
};


#endif